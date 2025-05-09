//dungeon.h

#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    EMPTY = 0,
    START,
    END,
    HAS_ITEM
} Attribute;

// I've decided to call it Node rather than Room for now.
typedef struct Node
{
    struct Node* north;
    struct Node* south;
    struct Node* east;
    struct Node* west;
    Attribute attribute;
    char* desc;
} Node;

typedef struct Level {
    struct Node room[10]; // Allocate 10 rooms
    struct Node* startNode;
    struct Node* endNode;
} Level;

#endif // DUNGEON_H
