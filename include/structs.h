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
	ENTITY_NONE,
	ENTITY_TOWER,
	ENTITY_TOWER_SHOT,
	ENTITY_ZOMBIE	
} EntityType;

typedef struct Cell {
	uint16_t count;
	entity_id_t* entites;
} cell_t;

typedef struct CollisionItem {
	entity_id_t a;
	entity_id_t b;
	Position* offset;
	struct CollisionItem* next;
} collision_item_t;

typedef struct Collision {
	collision_item_t* first;
	collision_item_t* last;
	int n;
} collision_t;

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

	Position* bounding_box;
	bool* alive;
	Position* positions;
	Size* sizes;
	float* rotations;
	EntityType* types;
	Velocity* velocities;
	Location* locations;
	Color* colors;

	void** entity_specific;

	float to_screen_space;

	collision_t collisions;

	Camera2D camera;
	Size screen;


	void(*physics_func)(struct World*, float deltaTime);
	void(*draw_func)(struct World*);

	entity_id_t level_data;

} world_t;

void init_grid(Size map_size, Size cell_size, uint16_t maxEntitiesPerCell);
void free_grid_components(grid_t* grid);

world_t* create_world(uint16_t entites, Size map_size, Size cell_size, Size screen, uint16_t maxEntitiesPerCell);
void free_world(world_t* world);


typedef struct Tower {
	float cooldown;
} tower_t;

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_STRUCTS_H
