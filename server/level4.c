#include "dungeon.h"
// Huy Pham

// External reference to the level array defined in main.c
extern Level level[4];

void initLevel4() // North, South, East, West
{
    level[3].room[9] = (Node){
        NULL,
        NULL,
        NULL,
        &level[3].room[8],
        END, strdup("Room 9: You stepped foot in an ornated room, with scripts undercipherable. To the end, there's a portal. You are unsure where it leads, but there are no other way out. Take the leap of faith.")};

    level[3].room[8] = (Node){
        NULL,
        NULL,
        &level[3].room[9],
        &level[3].room[3],
        EMPTY, strdup("Room 8: Praise be! is that a lit torch? Somebody was here! You must push forward.")};

    level[3].room[7] = (Node){
        &level[3].room[5],
        NULL,
        NULL,
        NULL,
        EMPTY, strdup("Room 7: A dead end! If you don't figure this out, you will perish just like him.")};

    level[3].room[6] = (Node){
        NULL,
        NULL,
        &level[3].room[6],
        NULL,
        EMPTY, strdup("Room 6: No, no! A dead end! There must be another way!")};

    level[3].room[5] = (Node){
        &level[3].room[4],
        &level[3].room[7],
        &level[3].room[6],
        NULL,
        HAS_ITEM, strdup("Room 5: You entered the room, following the curious echo. Alas, it was no beast, but a rat. \nThe scene was rather grim; The was gnawing on the remaining of an explorer. There's little flesh left, and seems to have signs of injury. It seems you weren't the first one to make it this far, but if you don't make it out, you'll just end up the same.\nThe remain was holding something - a treasure! Such irony, he made it this far to meet a gruesome end.")};

    level[3].room[4] = (Node){
        NULL,
        &level[3].room[5],
        NULL,
        &level[3].room[1],
        EMPTY, strdup("Room 4: A horrifying screech is ahead of you. Would you back off, or investigate?")};

    level[3].room[3] = (Node){
        &level[3].room[0],
        NULL,
        &level[3].room[8],
        &level[3].room[2],
        EMPTY, strdup("Room 3: There's two paths ahead. The footprint leads to a crack on the Northern wall. Would you follow it, or would it be wise to keep on course?")};

    level[3].room[2] = (Node){
        NULL,
        &level[3].room[1],
        &level[3].room[3],
        NULL,
        EMPTY, strdup("Room 2: Somebody was here, the footprint leads ahead. Could it be a chance to escape")};

    level[3].room[1] = (Node){
        &level[3].room[2],
        NULL,
        &level[3].room[4],
        &level[3].room[0],
        EMPTY, strdup("Room 1: The path splits into two. You are all alone, but can't shake the feeling of being watched.")};

    level[3].room[0] = (Node){
        NULL,
        NULL,
        &level[3].room[1],
        NULL,
        START, strdup("Room 0: You have entered a dank dungeon, there's only one path ahead.")};

    level[3].startNode = &level[3].room[0];
    level[3].endNode   = &level[3].room[9];
}