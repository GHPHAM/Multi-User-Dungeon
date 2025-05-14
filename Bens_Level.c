#include "dungeon.h"
// Ben Tzobery

// External reference to the level array defined in main.c
/* I don't know why I  did this, but I decided to do this for no reason. Its so stupid and so silli.

you go in a circle untill you reach the top, and then you can ethier go west or east. east rewards you with the end, while west gives you a shiny and sends you back to the start.
*/
extern Level level[4];

void initLevel1() // North, South, East, West
{
    level[0].room[9] = (Node){
        NULL,
        &level[0].room[0],
        &level[0].room[7],
        NULL,
        END, strdup("You reach the throne room with endless riches. /nCongradualtions!!!/n you can choose to go south to end up in the starting room, or you can explore the other door")};

    level[0].room[8] = (Node){
        NULL,
        &level[0].room[0],
        NULL,
        NULL,
        HAS_ITEM, strdup("You find a chest, which you open find an item. /nUnfortualnty, there is no exit except a hole that seems to take you back to the starting room.")};

    level[0].room[7] = (Node){
        &level[0].room[6],
        NULL,
        &level[0].room[8],
        &level[0].room[9],
        EMPTY, strdup("You reach the top of the maze, where you see two doors, one in the east and one in the west. /n You spy something shiny towards the east door")};

    level[0].room[6] = (Node){
        NULL,
        &level[0].room[7],
        NULL,
        &level[0].room[5],
        EMPTY, strdup("Your just out of breath as you see.....\n ....even more stairs (last one I promise).")};

    level[0].room[5] = (Node){
        NULL,
        &level[0].room[4],
        &level[0].room[6],
        NULL,
        EMPTY, strdup("You find more stairs. \nYou wonder why did the creator did this for.")};

    level[0].room[4] = (Node){
        &level[0].room[5],
        NULL,
        &level[0].room[3],
        NULL,
        EMPTY, strdup("You see more stairs as you enter the room. /nYou start to wonder why did they just not add an elevator to the maze. ")};

    level[0].room[3] = (Node){
        &level[0].room[2],
        NULL,
        NULL,
        &level[0].room[4],
        EMPTY, strdup("Guess whats at the top of the stairs?/n If you guess a straigth corridor, you would be incorect.")};

    level[0].room[2] = (Node){
        NULL,
        &level[0].room[3],
        NULL,
        &level[0].room[1],
        EMPTY, strdup("You find more stairs at the top of the stiars.\nYou start to wonder how tall this really is.")};

    level[0].room[1] = (Node){
        NULL,
        &level[0].room[0],
        &level[0].room[2],
        NULL,
        EMPTY, strdup("You start climbing up the stairs\n At the top of the stairs, you spy more stairs on your right.")};

    level[0].room[0] = (Node){
        level[0].room[1],
        NULL,
        NULL,
        NULL,
        START, strdup("Welcome to the start of this 'Mase'.\nAs you walk in, the door closes behind you, preventing your escape. \nYour only option is the set of stairs in the north")};

    level[0].startNode = &level[0].room[0];
    level[0].endNode   = &level[0].room[9];
}
