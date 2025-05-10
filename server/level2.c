#include "dungeon.h"
//By Hung Anh Ho

// External reference to the level array defined in main.c
extern Level level[4];

void initLevel2() // North, South, East, West
{
    level[0].room[9] = (Node){
        NULL,
        NULL,
        NULL,
        &level[0].room[8],
        END, strdup("Room 9: The Final Chamber. A faint glow emanates from a pedestal in the center. Could this be the end of your journey?")};

    level[0].room[8] = (Node){
        NULL,
        NULL,
        &level[0].room[9],
        &level[0].room[3],
        EMPTY, strdup("Room 8: The Hall of Echoes. Your footsteps reverberate endlessly, as if the walls are alive.")};

    level[0].room[7] = (Node){
        &level[0].room[5],
        NULL,
        NULL,
        NULL,
        EMPTY, strdup("Room 7: The Forgotten Alcove.\n Dust and cobwebs cover what appears to be an ancient mural.")};

    level[0].room[6] = (Node){
        NULL,
        NULL,
        &level[0].room[6],
        NULL,
        EMPTY, strdup("Room 6: The Mirror Room.\n Shards of broken mirrors litter the floor, reflecting distorted images of yourself.")};

    level[0].room[5] = (Node){
        &level[0].room[4],
        &level[0].room[7],
        &level[0].room[6],
        NULL,
        HAS_ITEM, strdup("Room 5: The Treasure Vault.\n A golden chest sits in the corner, but it looks like it’s been tampered with.")};

    level[0].room[4] = (Node){
        NULL,
        &level[0].room[5],
        NULL,
        &level[0].room[1],
        EMPTY, strdup("Room 4: The Crossroads.\n Four paths diverge here, each leading to an unknown fate.")};

    level[0].room[3] = (Node){
        NULL,
        NULL,
        &level[0].room[8],
        &level[0].room[2],
        EMPTY, strdup("Room 3: The Library of Whispers.\n Shelves of ancient tomes line the walls, and faint whispers fill the air.")};

    level[0].room[2] = (Node){
        NULL,
        &level[0].room[1],
        &level[0].room[3],
        NULL,
        EMPTY, strdup("Room 2: The Armory.\n Rusted weapons and broken shields are scattered across the floor.")};

    level[0].room[1] = (Node){
        &level[0].room[2],
        NULL,
        &level[0].room[4],
        &level[0].room[0],
        EMPTY, strdup("Room 1: The Guard Post.\n A long-abandoned station with overturned tables and broken chairs.")};

    level[0].room[0] = (Node){
        NULL,
        NULL,
        &level[0].room[1],
        NULL,
        START, strdup("Room 0: The Entrance Hall.\n The air is damp, and the faint smell of moss fills your nostrils.")};

    level[0].startNode = &level[0].room[0];
    level[0].endNode   = &level[0].room[9];
}