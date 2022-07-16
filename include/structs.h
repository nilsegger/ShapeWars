#ifndef INCLUDE_STRUCTS_H
#define INCLUDE_STRUCTS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y;
} Position, Size, Velocity;

typedef struct {

	int top_left;
	int top_right;
	int bottom_left;
	int bottom_right;

} Location;

typedef enum {
    None,
    Walks 
} EntityType;

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_STRUCTS_H
