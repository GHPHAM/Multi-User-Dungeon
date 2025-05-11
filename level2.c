#include "dungeon.h"
// By Hung Anh Ho

// External reference to the level array defined in main.c
extern Level level[4];

#include "dungeon.h"
// By Hung Anh Ho

// External reference to the level array defined in main.c
extern Level level[4];

// Map of Level 2:
/*
    The map is a simple representation of the dungeon layout.
                                            [Room 9: Ball Room]
                                                    |
                                        [Room 7: Passage to Room 9]
                                                    |
 [Room 5: Trap Room] <---------------------[Room 4: Large Hall] <--> [Room 6: Treasure Room]
        |                                           |
        |     [Room 8: Side Passage] <--> [Room 3: Guard Watch]
        |                                           |
        |                                [Room 2: Broken Chamber]
        |                                           |
        --------->  [Room 0: Entrance]  <--> [Room 1: Tunnel]

COMMENTS:
- Vertical lines (|) represent North-South connections.
- Horizontal lines (<->) represent East-West connections.
- Room 6 contains the item (HAS_ITEM).
- Room 0 is the starting point (START).
- Room 9 is the end point (END)-connector room.
*/

void initLevel2() // North, South, East, West
{
    // Room 9: End Room (Goal)
    level[1].room[9] = (Node){
        NULL,
        &level[1].room[7], // South to Room 7
        NULL,
        NULL,
        END, strdup("Room 9: The Ball Room.\n A large door stands before you,\n it is closed, marking a dead-end end of your journey.")};

    // Room 8: Side Passage
    level[1].room[8] = (Node){
        NULL,
        NULL,
        &level[1].room[3], // East to Room 3
        NULL,
        EMPTY, strdup("Room 8: The Side Passage.\n A narrow corridor leading to an cemetery with an weeping angel.")};

    // Room 7: Passage to Room 9
    level[1].room[7] = (Node){
        &level[1].room[9], // North to Room 9
        &level[1].room[4], // South to Room 4
        NULL,
        NULL,
        EMPTY, strdup("Room 7: The Passage.\n A dimly lit hallway leading to the a giant room.")};

    // Room 6: Treasure Room
    level[1].room[6] = (Node){
        NULL,
        NULL,
        NULL,
        &level[1].room[4], // West to Room 4
        HAS_ITEM, strdup("Room 6: The Treasure Room.\n A golden chest sits in the center,\n surrounded by faint glowing runes.")};

    // Room 5: Trap Door
    level[1].room[5] = (Node){
        NULL,
        NULL,
        &level[1].room[0], // East to Room 0 (trap sends you back)
        NULL,              // Not going back West to Room 4
        EMPTY, strdup("Room 5: The Trap Door.\n As you step inside, the floor collapses,\n and you fall back to the entrance!")};

    // Room 4: Large Hall
    level[1].room[4] = (Node){
        &level[1].room[7], // North to Room 7
        &level[1].room[3], // South to Room 3
        &level[1].room[6], // East to Room 6
        &level[1].room[5], // West to Room 5 (trap)
        EMPTY, strdup("Room 4: The Large Hall.\n A grand room with multiple paths branching out.\n There are 3 doors ahead of you.\n! !!CHOOSE WISELY!!\n")};

    // Room 3: Guard Watch
    level[1].room[3] = (Node){
        &level[1].room[4], // North to Room 4
        &level[1].room[2], // South to Room 2
        NULL,
        &level[1].room[8], // West to Room 8
        EMPTY, strdup("Room 3: The Guard Watch.\n A small room with remnants of an old guard post.")};

    // Room 2: Broken Chamber
    level[1].room[2] = (Node){
        &level[1].room[3], // North to Room 3
        &level[1].room[1], // South to Room 1
        NULL,
        NULL,
        EMPTY, strdup("Room 2: The Broken Chamber.\n Cracks run along the walls,\n and debris litters the floor.")};

    // Room 1: Tunnel
    level[1].room[1] = (Node){
        &level[1].room[2], // North to Room 2
        &level[1].room[0], // South to Room 0
        NULL,
        NULL,
        EMPTY, strdup("Room 1: The Tunnel.\n A narrow passage leading deeper into the dungeon.")};

    // Room 0: Entrance
    level[1].room[0] = (Node){
        NULL,
        NULL,
        &level[1].room[1], // East to Room 1
        NULL,
        START, strdup("Room 0: The Entrance.\n The air is damp,\n and the faint smell of moss fills your nostrils.")};

    level[1].startNode = &level[1].room[0];
    level[1].endNode = &level[1].room[9];
}