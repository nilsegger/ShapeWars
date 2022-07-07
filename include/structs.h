#ifndef INCLUDE_STRUCTS_H
#define INCLUDE_STRUCTS_H

typedef struct {
    float x, y;
} Position, Size, Velocity;

typedef struct {
    int top_left;
    int top_right;
    int bot_left;
    int bot_right;
} CellLocation;

#endif // INCLUDE_STRUCTS_H
