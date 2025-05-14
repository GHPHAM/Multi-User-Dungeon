// main.c
#include "dungeon.h"
#include "mqtt.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

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

    // Otherwise get local keyboard input
    debugMessage("\nWhere would you like to go? (^/v/</>/r): ");
    scanf(" %c", &command);

    clearConsole();
    return tolower(command);
}

// Free allocated memory
void cleanupLevel()
{
    for (int i = 0; i < sizeof(level); ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            free(level->room[i].desc);
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

    Node *currentRoom;
    char command;
    int playing = 1;

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

    // Main game loop
    while (playing)
    {
        displayRoom(currentRoom);

        /*
        // Check if we're at the end
        if (currentRoom->attribute == HAS_ITEM)
        {
            printf("\nCongratulations! You have found the item!\n");
            printf("Would you like to continue exploring? (y/n): ");
            scanf(" %c", &command);
            if (tolower(command) != 'y')
                break;
        }
        */

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
        {
            debugMessage("\nThanks for playing!\n");
            debugMessage("\nRestarting game\n");
            // Clean up allocated memory
            cleanupLevel();
            main();
            break;
        }
        default:
            debugMessage("\nInvalid command. Use ^, v, <, >, or r.\n");
            break;
        }
    }

    // Clean up allocated memory
    cleanupLevel();

    return 0;
}