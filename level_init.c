#include "dungeon.h"

// External reference to the level array defined in main.c
extern Level level[4];

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