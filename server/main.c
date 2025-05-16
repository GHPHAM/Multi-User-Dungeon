// main.c
#include "dungeon.h"
#include "mqtt.h"
#include "tcp_server.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>

///////////////// TCP SOCKET /////////////////////

#define SERVER_PORT 8888
#define MAX_CONNECTIONS 5
#define MAX_LEN 1024

pthread_cond_t command_available = PTHREAD_COND_INITIALIZER;

// Global socket server
static tcp_server_t* server = NULL;
static pthread_t socket_thread;
static volatile int server_running = 0;
static char last_command = 0;
static pthread_mutex_t command_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to initialize the socket server
int initSocketServer() {
    // Configure the server
    tcp_server_config_t config = {
        .port = SERVER_PORT,
        .max_connections = MAX_CONNECTIONS,
        .buffer_size = MAX_LEN
    };

    // Initialize the server
    server = tcp_server_init_with_config(&config);
    if (server == NULL) {
        fprintf(stderr, "Failed to initialize socket server\n");
        return -1;
    }

    // Start the server
    if (tcp_server_start(server) != 0) {
        fprintf(stderr, "Failed to start socket server\n");
        tcp_server_cleanup(server);
        server = NULL;
        return -1;
    }

    printf("Socket server initialized on port %d\n", SERVER_PORT);
    return 0;
}

// Socket listener thread function
void* socketListenerThread(void* arg) {
    printf("Socket listener thread started\n");

    while (server_running) {
        // Accept a client connection
        printf("Waiting for client connection...\n");
        if (tcp_server_accept(server) != 0) {
            fprintf(stderr, "Failed to accept client connection\n");
            sleep(1);
            continue;
        }

        printf("Client connected\n");

        // Process client commands
        while (server_running) {
            int bytes_received = tcp_server_receive(server);
            if (bytes_received <= 0) {
                printf("Client disconnected\n");
                break;
            }

            const char* msg = tcp_server_get_message(server);
            if (msg != NULL && strlen(msg) > 0) {
                char cmd = msg[0];

                // Check if it's a valid command
                if (cmd == 'w' || cmd == 's' || cmd == 'a' || cmd == 'd' || cmd == 'r' ||
                    cmd == 'W' || cmd == 'S' || cmd == 'A' || cmd == 'D' || cmd == 'R') {

                    // Store the command
                    pthread_mutex_lock(&command_mutex);
                    last_command = tolower(cmd);
                    // Signal that a command is available
                    pthread_cond_signal(&command_available);
                    pthread_mutex_unlock(&command_mutex);

                    // Confirm receipt of command
                    char confirm[50];
                    snprintf(confirm, sizeof(confirm), "Command received: %c\n", cmd);
                    tcp_server_send(server, confirm, strlen(confirm));
                }
            }
        }

        // Disconnect client when done
        tcp_server_disconnect_client(server);
    }

    printf("Socket listener thread exiting\n");
    return NULL;
}

// Start the socket listener
int startSocketListener() {
    // Initialize the socket server
    if (initSocketServer() != 0) {
        return -1;
    }

    // Set the running flag
    server_running = 1;

    // Create the listener thread
    if (pthread_create(&socket_thread, NULL, socketListenerThread, NULL) != 0) {
        fprintf(stderr, "Failed to create socket listener thread\n");
        tcp_server_cleanup(server);
        server = NULL;
        server_running = 0;
        return -1;
    }

    return 0;
}

// Stop the socket listener
void stopSocketListener() {
    if (!server_running) {
        return;
    }

    // Stop the thread
    server_running = 0;

    // Wait for the thread to exit
    pthread_join(socket_thread, NULL);

    // Clean up
    if (server != NULL) {
        tcp_server_cleanup(server);
        server = NULL;
    }

    printf("Socket listener stopped\n");
}

// Get the latest command from the socket (replaces getCommand in main.c)
char getSocketCommand() {
    char cmd = 0;

    // Lock the mutex for command checking
    pthread_mutex_lock(&command_mutex);

    // If no command is available, wait for one
    if (last_command == 0) {
        // Wait for a signal that a command is available
        // Add a timeout of 1 second to allow for program termination
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 1;  // 1 second timeout

        // Wait for command or timeout
        pthread_cond_timedwait(&command_available, &command_mutex, &timeout);
    }

    // Get the command
    cmd = last_command;
    last_command = 0;  // Reset after reading

    // Unlock the mutex
    pthread_mutex_unlock(&command_mutex);

    return cmd;
}

//////////////////////////////////////////////////
int input_pipe[2];

// Global level array, accessible by level_init.c
Level level[4];
void initLevel1();
void initLevel2();
void initLevel3();
void initLevel4();

// Calling the level to initialize
void initLevels()
{
    initLevel1();
    initLevel2();
    initLevel3();
    initLevel4();
    //...
}

void clearConsole()
{
    printf("\033[H\033[J");
}

void debugMessage(const char *format, ...)
{
    char buffer[512];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    publishMessage(buffer);
    // printf("%s", buffer);
}

// Display room information and available exits
void displayRoom(Node *currentRoom)
{
    debugMessage("\n=== %s ===\n", currentRoom->desc);

    debugMessage("Exits: ");
    if (currentRoom->north)
        debugMessage("North ");
    if (currentRoom->south)
        debugMessage("South ");
    if (currentRoom->east)
        debugMessage("East ");
    if (currentRoom->west)
        debugMessage("West ");
    debugMessage("\n");

    // Display special room attributes
    switch (currentRoom->attribute)
    {
    case START:
        debugMessage("This is the starting point.\n");
        break;
    case END:
        debugMessage("This is the exit of the dungeon!\n");
        break;
    case HAS_ITEM:
        debugMessage("There's something interesting here.\n");
        currentRoom->attribute = EMPTY; // Mark as visited
        break;
    default:
        break;
    }
}

// Get user input for movement
char getCommand()
{
    char command = 0;

    debugMessage("\nWhere would you like to go? (^/v/</>/r): ");

    // Get command from socket instead of keyboard
    while (command == 0) {
        // Check for socket command
        command = getSocketCommand();

        // If no command yet, sleep a bit before checking again
        if (command == 0) {
            usleep(200000); // Sleep for 200ms to reduce CPU usage
        }
    }

    return command;
}

// Free allocated memory
void cleanupLevel()
{
    for (int i = 0; i < sizeof(level) / sizeof(Level); ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            if (level[i].room[j].desc != NULL) {
                free(level[i].room[j].desc);
                level[i].room[j].desc = NULL;
            }
        }
    }
}

// shuffling the number
void shuffle(short int *array, int size)
{
    srand(time(NULL));
    for (int i = size - 1; i > 0; --i)
    {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }

    // debug
    printf("Run seed: ");
    for (int i = 0; i < size; ++i)
    {
        printf("%d", array[i]);
    }
    printf("\n");
}

void cleanup()
{
    stopListener();
    printf("Thanks for playing!\n");
}

// Signal handler for graceful termination
void signalHandler(int sig)
{
    printf("\nReceived signal %d. Exiting...\n", sig);
    cleanup();
    exit(0);
}

int main()
{
    char input[20];

    // Creating input pipe
    if (pipe(input_pipe) == -1)
    {
        perror("pipe_failed");
        exit(EXIT_FAILURE);
    }
    fcntl(input_pipe[1], F_SETFL, O_NONBLOCK); // non-blocking write end

    dup2(input_pipe[0], STDIN_FILENO);
    close(input_pipe[0]);

    // Set up signal handlers for graceful termination
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Register cleanup function to be called on normal exit
    atexit(cleanup);

    // Start the MQTT listener
    startListener();

    /////////////////////////////////////

    // Game loop
    while (1) {  // Outer infinite loop for game restarts
        Node *currentRoom;
        char command;
        int playing = 1;
        int restart = 0;

        printf("Welcome to the Dungeon Crawler!\n");
        printf("Commands: ^, v, <, >, r (restart)\n");

        // Initialize the levels
        initLevels();

        // Randomizing the queue of levels
        short int levelOrder[] = {0, 1, 2, 3};
        short int size = sizeof(levelOrder) / sizeof(levelOrder[0]);
        shuffle(levelOrder, size);

        // Joining levels
        for (int i = 0; i < size - 1; ++i)
        {
            level[levelOrder[i]].room[9].east = &level[levelOrder[i + 1]].room[0];
            level[levelOrder[i + 1]].room[0].west = &level[levelOrder[i]].room[9];
        }

        // Start at the beginning
        currentRoom = level[levelOrder[0]].startNode;

        // Inner game loop
        while (playing)
        {
            displayRoom(currentRoom);

            // Get command
            command = getCommand();

            // Process command
            switch (command)
            {
            case 'w':
                if (currentRoom->north)
                {
                    currentRoom = currentRoom->north;
                    debugMessage("\nYou move north.\n");
                }
                else
                {
                    debugMessage("\nYou can't go that way.\n");
                }
                break;
            case 's':
                if (currentRoom->south)
                {
                    currentRoom = currentRoom->south;
                    debugMessage("\nYou move south.\n");
                }
                else
                {
                    debugMessage("\nYou can't go that way.\n");
                }
                break;
            case 'd':
                if (currentRoom->east)
                {
                    currentRoom = currentRoom->east;
                    debugMessage("\nYou move east.\n");
                }
                else
                {
                    debugMessage("\nYou can't go that way.\n");
                }
                break;
            case 'a':
                if (currentRoom->west)
                {
                    currentRoom = currentRoom->west;
                    debugMessage("\nYou move west.\n");
                }
                else
                {
                    debugMessage("\nYou can't go that way.\n");
                }
                break;
            case 'r':
                debugMessage("\nRestarting game...\n");
                playing = 0;  // Exit inner game loop
                restart = 1;  // Set restart flag
                break;
            default:
                debugMessage("\nInvalid command. Use w (north), s (south), a (west), d (east), or r (restart).\n");
                break;
            }

            // Check if we've reached the end
            if (currentRoom->attribute == END) {
                debugMessage("\nCongratulations! You've found the exit!\n");
                debugMessage("Would you like to play again? (y/n): ");

                char playAgain = 0;
                int timeout = 50; // 5 seconds timeout (50 * 100ms)

                while (timeout > 0 && playAgain == 0) {
                    playAgain = getSocketCommand();
                    if (playAgain == 0) {
                        usleep(100000); // 100ms
                        timeout--;
                    }
                }

                if (playAgain == 'y' || playAgain == 'Y') {
                    playing = 0;  // Exit inner game loop
                    restart = 1;  // Set restart flag
                } else {
                    playing = 0;  // Exit inner game loop
                    restart = 0;  // Don't restart
                }
            }
        }

        // Clean up allocated memory before potentially restarting
        cleanupLevel();

        if (!restart) {
            break;  // Exit the outer game loop if not restarting
        }
    }

    return 0;
}