// main.c
#include "dungeon.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// Display room information and available exits
void displayRoom(Node *currentRoom)
{
    printf("\n=== %s ===\n", currentRoom->desc);

    printf("Exits: ");
    if (currentRoom->north)
        printf("North ");
    if (currentRoom->south)
        printf("South ");
    if (currentRoom->east)
        printf("East ");
    if (currentRoom->west)
        printf("West ");
    printf("\n");

    // Display special room attributes
    switch (currentRoom->attribute)
    {
    case START:
        printf("This is the starting point.\n");
        break;
    case END:
        printf("This is the exit of the dungeon!\n");
        break;
    case HAS_ITEM:
        printf("There's something interesting here.\n");
        break;
    default:
        break;
    }
}

// Get user input for movement
char getCommand()
{
    char command;
    printf("\nWhere would you like to go? (w/a/s/d/q): ");
    scanf(" %c", &command);
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

int main()
{
    Node *currentRoom;
    char command;
    int playing = 1;

    printf("Welcome to the Dungeon Crawler!\n");
    printf("Commands: w a s d, q (quit)\n");

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

        // Check if we're at the end
        if (currentRoom->attribute == HAS_ITEM)
        {
            printf("\nCongratulations! You have found the item!\n");
            currentRoom->attribute = EMPTY; // Mark the item as found so when continuing exploring should not be found again

            do // Input validation
            {
                printf("Would you like to continue exploring? (y/n): ");
                command = getchar();
                while (getchar() != '\n')
                    ; // clear extra buffer
                if (tolower(command) == 'y')
                    break;

                else if (tolower(command) == 'n')
                {
                    printf("\nThank you for playing\n");
                    cleanupLevel();
                    exit(0);
                }

                else
                {
                    printf("\nInvalid input. Please enter 'y' or 'n'.\n");
                }

            } while (1);
        }

        // Get command
        command = getCommand();

        // Process command
        switch (command)
        {
        case 'w':
            if (currentRoom->north)
            {
                currentRoom = currentRoom->north;
                printf("\nYou move north.\n");
            }
            else
            {
                printf("\nYou can't go that way.\n");
            }
            break;
        case 's':
            if (currentRoom->south)
            {
                currentRoom = currentRoom->south;
                printf("\nYou move south.\n");
            }
            else
            {
                printf("\nYou can't go that way.\n");
            }
            break;
        case 'd':
            if (currentRoom->east)
            {
                currentRoom = currentRoom->east;
                printf("\nYou move east.\n");
            }
            else
            {
                printf("\nYou can't go that way.\n");
            }
            break;
        case 'a':
            if (currentRoom->west)
            {
                currentRoom = currentRoom->west;
                printf("\nYou move west.\n");
            }
            else
            {
                printf("\nYou can't go that way.\n");
            }
            break;
        case 'q':
            printf("\nThanks for playing!\n");
            playing = 0;
            break;
        default:
            printf("\nInvalid command. Use w, a, s, d, or q.\n");
            break;
        }
    }

    // Clean up allocated memory
    cleanupLevel();

    return 0;
}