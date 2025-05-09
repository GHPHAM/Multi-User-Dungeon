// main.c
#include "dungeon.h"

// Global level array, accessible by level_init.c
Level level[4];

// Function declarations - now defined in level_init.c
void initLevels();

// Display room information and available exits
void displayRoom(Node* currentRoom) {
    printf("\n=== %s ===\n", currentRoom->desc);

    printf("Exits: ");
    if (currentRoom->north) printf("North ");
    if (currentRoom->south) printf("South ");
    if (currentRoom->east) printf("East ");
    if (currentRoom->west) printf("West ");
    printf("\n");

    // Display special room attributes
    switch(currentRoom->attribute) {
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
char getCommand() {
    char command;
    printf("\nWhere would you like to go? (w/a/s/d/q): ");
    scanf(" %c", &command);
    return tolower(command);
}

// Free allocated memory
void cleanupLevel(Level* level) {
    for (int i = 0; i < 10; i++) {
        free(level->room[i].desc);
    }
}

int main() {
    Node* currentRoom;
    char command;
    int playing = 1;

    printf("Welcome to the Dungeon Crawler!\n");
    printf("Commands: w a s d, q (quit)\n");

    // Initialize the levels
    initLevels();

    // Start at the beginning
    currentRoom = level[0].startNode;

    // Main game loop
    while (playing) {
        displayRoom(currentRoom);

        // Check if we're at the end
        if (currentRoom->attribute == HAS_ITEM) {
            printf("\nCongratulations! You have found the item!\n");
            printf("Would you like to continue exploring? (y/n): ");
            scanf(" %c", &command);
            if (tolower(command) != 'y')
                break;
        }

        // Get command
        command = getCommand();

        // Process command
        switch(command) {
            case 'w':
                if (currentRoom->north) {
                    currentRoom = currentRoom->north;
                    printf("\nYou move north.\n");
                } else {
                    printf("\nYou can't go that way.\n");
                }
                break;
            case 's':
                if (currentRoom->south) {
                    currentRoom = currentRoom->south;
                    printf("\nYou move south.\n");
                } else {
                    printf("\nYou can't go that way.\n");
                }
                break;
            case 'd':
                if (currentRoom->east) {
                    currentRoom = currentRoom->east;
                    printf("\nYou move east.\n");
                } else {
                    printf("\nYou can't go that way.\n");
                }
                break;
            case 'a':
                if (currentRoom->west) {
                    currentRoom = currentRoom->west;
                    printf("\nYou move west.\n");
                } else {
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
    cleanupLevel(&level[0]);

    return 0;
}