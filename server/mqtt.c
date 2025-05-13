#include "mqtt.h"

// Just use the commands directly from PATH
char mosquittoPath[] = "mosquitto_pub";
char mosquittoSub[] = "mosquitto_sub";

// Global variables
int mqtt_pipe_fd[2] = {-1, -1};
pid_t mqtt_sub_pid = -1;
int listener_running = 0;

char currentMove = 0; // W A S D

// MQTT Configuration
#define MQTT_HOST "" // Add your MQTT broker address here
#define MQTT_TOPIC "MUD"


void mqttUpdate(const char *topic, const char *message) {
    char subTopic[256];

    // Check for current move update
    snprintf(subTopic, sizeof(subTopic), "%s/moves", MQTT_TOPIC);
    if (strcmp(topic, subTopic) == 0) {
        currentMove = message[0];
        return;
    }
}

char getMQTTInput() {
    char move = currentMove;
    currentMove = 0; // Reset after reading
    return move;
}

// Thread function to read from the pipe and process MQTT messages
void *mqttListenerThread(void *arg) {
    char buffer[1024];
    ssize_t bytesRead;
    static char leftover[1024] = "";
    FILE *fp = fdopen(mqtt_pipe_fd[0], "r");

    if (fp == NULL) {
        perror("fdopen failed");
        return NULL;
    }

    while (listener_running) {
        // Read a line from the pipe
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            if (feof(fp)) {
                break; // End of file
            }
            usleep(100000);  // 100ms
            continue;
        }

        // Remove trailing newline
        buffer[strcspn(buffer, "\n")] = '\0';

        // Skip empty lines
        if (strlen(buffer) == 0) {
            continue;
        }

        // Parse the line: format is "topic message"
        char *space = strchr(buffer, ' ');
        if (space) {
            *space = '\0';  // Split topic and message
            char *topic = buffer;
            char *message = space + 1;

            // Process the message
            mqttUpdate(topic, message);
        }

        usleep(100000);  // 100ms, don't monopolize CPU
    }

    fclose(fp);
    return NULL;
}

// Publish a message to the MQTT broker
void publishMessage(const char *message) {
    char command[512];
    printf("Sending: %s\n", message);

    // Create the command string
    snprintf(command, sizeof(command),
             "%s -h %s -t %s -m \"%s\" > /dev/null 2>&1",
             mosquittoPath, MQTT_HOST, MQTT_TOPIC, message);

    // Execute the command using system
    system(command);

    // Sleep briefly to allow time for the message to be processed
    usleep(100000);  // 100ms in microseconds
}

// Start the MQTT subscriber process
void startListener() {
    if (listener_running) {
        return;
    }

    // Create pipe for reading subscriber output
    if (pipe(mqtt_pipe_fd) == -1) {
        perror("pipe failed");
        return;
    }

    // Fork to create child process
    mqtt_sub_pid = fork();

    if (mqtt_sub_pid < 0) {
        perror("fork failed");
        close(mqtt_pipe_fd[0]);
        close(mqtt_pipe_fd[1]);
        return;
    }

    if (mqtt_sub_pid == 0) {
        // Child process - will run mosquitto_sub

        // Close read end of pipe
        close(mqtt_pipe_fd[0]);

        // Redirect stdout to pipe
        dup2(mqtt_pipe_fd[1], STDOUT_FILENO);
        close(mqtt_pipe_fd[1]);

        // Execute mosquitto_sub
        char topic_arg[100];
        snprintf(topic_arg, sizeof(topic_arg), "%s/#", MQTT_TOPIC);

        // Since we're using commands from PATH, we need to use the first argument as the program name
        execlp(mosquittoSub, mosquittoSub, "-h", MQTT_HOST, "-t", topic_arg, "-v", NULL);

        // If execl returns, there was an error
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    // Parent process continues here

    // Close write end of pipe in parent
    close(mqtt_pipe_fd[1]);
    mqtt_pipe_fd[1] = -1;

    // Start a thread that reads from the pipe
    listener_running = 1;

    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, mqttListenerThread, NULL) != 0) {
        perror("pthread_create failed");
        stopListener();
        return;
    }

    // Detach the thread so its resources are freed automatically
    pthread_detach(thread_id);

    printf("MQTT subscriber started\n");
}

// Stop the MQTT subscriber process
void stopListener() {
    if (!listener_running) {
        return;
    }

    listener_running = 0;

    // Terminate the mosquitto_sub process
    if (mqtt_sub_pid > 0) {
        kill(mqtt_sub_pid, SIGTERM);
        waitpid(mqtt_sub_pid, NULL, 0);
        mqtt_sub_pid = -1;
    }

    // Close the pipe
    if (mqtt_pipe_fd[0] >= 0) {
        close(mqtt_pipe_fd[0]);
        mqtt_pipe_fd[0] = -1;
    }

    if (mqtt_pipe_fd[1] >= 0) {
        close(mqtt_pipe_fd[1]);
        mqtt_pipe_fd[1] = -1;
    }

    printf("MQTT listener stopped\n");
}
