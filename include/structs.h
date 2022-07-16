#ifndef INCLUDE_STRUCTS_H
#define INCLUDE_STRUCTS_H

#include <stdint.h>
#include <raylib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t entity_id_t; 

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

typedef struct {
	uint16_t count;
	entity_id_t* entites;
} cell_t;

typedef struct Grid {
	uint16_t max_entitites_per_cell;

	Size map_size;
	Size cell_size;

	uint16_t rows;
	uint16_t cols;

	uint16_t count;

	cell_t* cells;
} grid_t;

typedef struct World {
	grid_t grid;
	uint16_t entities_count;
	EntityType* types;
	Position* positions;
	Size* sizes;
	Velocity* velocities;
	Location* locations;
	Color* colors;
} world_t;

void init_grid(Size map_size, Size cell_size, uint16_t maxEntitiesPerCell);
void free_grid_components(grid_t* grid);

world_t* create_world(uint16_t entites, Size map_size, Size cell_size, uint16_t maxEntitiesPerCell);
void free_world(world_t* world);

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_STRUCTS_H
