// main.c
#include "dungeon.h"

Level level[4];

void initLevel1() // North, South, East, West
{
    level[0].room[9] = (Node){
        NULL,
        NULL,
        NULL,
        &level[0].room[8],
        END, strdup("Room 9")};

    level[0].room[8] = (Node){
        NULL,
        NULL,
        &level[0].room[9],
        &level[0].room[3],
        EMPTY, strdup("Room 8")};

    level[0].room[7] = (Node){
        &level[0].room[5],
        NULL,
        NULL,
        NULL,
        EMPTY, strdup("Room 7")};

    level[0].room[6] = (Node){
        NULL,
        NULL,
        &level[0].room[6],
        NULL,
        EMPTY, strdup("Room 6")};

    level[0].room[5] = (Node){
        &level[0].room[4],
        &level[0].room[7],
        &level[0].room[6],
        NULL,
        HAS_ITEM, strdup("Room 5")};

    level[0].room[4] = (Node){
        NULL,
        &level[0].room[5],
        NULL,
        &level[0].room[1],
        EMPTY, strdup("Room 4")};

    level[0].room[3] = (Node){
        NULL,
        NULL,
        &level[0].room[8],
        &level[0].room[2],
        EMPTY, strdup("Room 3")};

    level[0].room[2] = (Node){
        NULL,
        &level[0].room[1],
        &level[0].room[3],
        NULL,
        EMPTY, strdup("Room 2")};

    level[0].room[1] = (Node){
        &level[0].room[2],
        NULL,
        &level[0].room[4],
        &level[0].room[0],
        EMPTY, strdup("Room 1")};

    level[0].room[0] = (Node){
        NULL,
        NULL,
        &level[0].room[1],
        NULL,
        START, strdup("Room 0")};

    level[0].startNode = &level[0].room[0];
    level[0].endNode   = &level[0].room[9];
}

void initLevels()
{
    initLevel1();
    //initLevel2();
    //...
}

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