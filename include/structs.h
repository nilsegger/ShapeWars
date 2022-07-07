#ifndef INCLUDE_STRUCTS_H
#define INCLUDE_STRUCTS_H

typedef struct {
    float x, y;
} Position, Size, Velocity;

typedef struct {

	int top_left;
	int top_right;
	int bottom_left;
	int bottom_right;

} CellLocation;


#endif // INCLUDE_STRUCTS_H
