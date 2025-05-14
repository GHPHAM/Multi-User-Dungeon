#include "dungeon.h"
// By Anthony Penaloza

// External reference to the level array defined in main.c
extern Level level[4];
// Map of Level 3:
/*
    A visual representation of my dungeon layout.

                                                         [Room 9: Closet ]
                                                                 |
[Room 8: Large Bedroom] <-> [Room 5: Stair case] <-> [Room 7: Small Bedroom]
                                 |
[Room 6:Mirror Room]<-> [Room 3: Living Room]  <-> [Room 4: Dining Room]
                                |                            
                [Room 2: Room with Paintings ] 
                                |
--> [Room 0: Start] <-> [Room 1: Hallway]
Notes:
Vertical lines (|) North/South
Horizontal Lines (<->) East/West
Room 0 is (START)
Room 9 is (END)
Room 6 has the item
*/

void initLevel3() // North, South, East, West
{
    level[2].room[9] = (Node){
        NULL,
        &level[2].room[7], //South to Room 7
        NULL,
        NULL,
        END, strdup("A small closet filled with an extensive collection of high-end coats.")};

    level[2].room[8] = (Node){
        NULL,
        NULL,
        &level[2].room[5], //East to Room 5
        NULL,
        EMPTY, strdup("An expansive bedroom featuring opulently decorated furnishings and a spacious bed.")};

    level[2].room[7] = (Node){
        &level[2].room[9], //North to Room 9
        NULL,
        NULL,
        &level[2].room[5], //West to Room 5 
        EMPTY, strdup("A compact bedroom featuring a closet and a modest bed.")};

    level[2].room[6] = (Node){
        NULL,
        NULL,
        &level[2].room[3], //East to Room 3
        NULL,
        HAS_ITEM, strdup("You are confronted with a labyrinth of mirrors,\n At the end of the labyrinth,\n You find the item you were looking for!.")};

    level[2].room[5] = (Node){
        NULL,
        &level[2].room[3], //South to Room 3
        &level[2].room[7], //East to Room 7
        &level[2].room[8], //West to Room 8
        HAS_ITEM, strdup("You are presented with a grand staircase that branches off in two directions.")};

    level[2].room[4] = (Node){
        NULL,
        NULL,
        NULL,
        &level[2].room[3], //West to Room 3
        EMPTY, strdup("Before you lies a dining hall that offers a wide variety of food.")};

    level[2].room[3] = (Node){
        &level[2].room[5], //North to Room 5
        &level[2].room[2], //South to Room 2
        &level[2].room[4], //East to Room 4
        &level[2].room[6], //West to Room 6
        EMPTY, strdup("In front of you is a living room featuring a fireplace.")};

    level[2].room[2] = (Node){
        &level[2].room[3], //North to Room 3
        &level[2].room[1], //South to Room 1
        NULL,
        NULL,
        EMPTY, strdup("Presented is a room with abstract paintings all over the walls.")};

    level[2].room[1] = (Node){
        &level[2].room[2], //North to Room 2
        NULL,
        NULL,
        &level[2].room[0], //West to Room 0
        EMPTY, strdup("In front of you is a long hallway with a door at the end.")};

    level[2].room[0] = (Node){
        NULL,
        NULL,
        &level[2].room[1], //East to Room 1
        NULL,
        START, strdup("The mansion's grand entrance has heavy doors that are hard to open.")};

    level[2].startNode = &level[2].room[0];
    level[2].endNode   = &level[2].room[9];
}
