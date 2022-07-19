#ifndef INCLUDE_CELLS_H
#define INCLUDE_CELLS_H

/*
#define MAP_WIDTH 100 // 100m
#define MAP_HEIGHT 200 // 1km

#define CELL_WIDTH 10 
#define CELL_HEIGHT 10 

#define CELL_ROWS (MAP_HEIGHT / CELL_HEIGHT)
#define CELL_COLS (MAP_WIDTH / CELL_WIDTH)

// Should be max 65536
#define CELLS_COUNT (MAP_WIDTH / CELL_WIDTH * MAP_HEIGHT / CELL_HEIGHT)

// THERE IS A BUG WHERE IF MAX_ENTITES_PER_CELL IS BIGGER THAN ENTITES THE PROGRAM WILL CRASH?
#define MAX_ENTITES_PER_CELL 100
*/

#ifdef __cplusplus
extern "C" {
#endif

void cells_begin_track_entites(world_t* world);
void cells_track_entity(world_t* world, entity_id_t entity);
Position cell_position(world_t* world, int index);
int cell_row(world_t* world, int index);
int cell_col(world_t* world, int index);
int cell_row_first(world_t* world, int row);
int cell_row_last(world_t* world, int row);
int cell_single_index(world_t* world, Position* position);
int cell_neighbour_top_left(world_t* world, int index);
int cell_neighbour_top(world_t* world, int index);
int cell_neighbour_top_right(world_t* world, int index);
int cell_neighbour_left(world_t* world, int index);
int cell_neighbour_right(world_t* world, int index);
int cell_neighbour_bottom_left(world_t* world, int index);
int cell_neighbour_bottom(world_t* world, int index);
int cell_neighbour_bottom_right(world_t* world, int index);

inline void cells_rectangle_location(world_t* world, entity_id_t entity, Location* location) {

	Position* bl = &world->bounding_box[entity * 2];
	Position* tr = &world->bounding_box[entity * 2 + 1];

	Position br = {tr->x, bl->y};
	Position tl = {bl->x, tr->y};

	location->bottom_left = cell_single_index(world, bl);

	location->bottom_right = cell_single_index(world, &br);
    if (location->bottom_right == location->bottom_left) location->bottom_right = -1;

	location->top_left = cell_single_index(world, &tl);
    if (location->top_left == location->bottom_left
        || location->top_left == location->bottom_right) location->top_left = -1;

	location->top_right = cell_single_index(world, tr);
    if (location->top_right == location->bottom_left
		|| location->top_right== location->bottom_right
        || location->top_right == location->top_left) location->top_right = -1;
}

inline bool cell_location_contains(Location* location, int cell) {
	return location->top_left == cell
		|| location->top_right == cell
		|| location->bottom_left == cell
		|| location->bottom_right == cell;
}

inline bool cell_location_unique(Location* location, int* compareTo) {
	bool unique = true;
	if (&location->top_left != compareTo)
		unique = unique && location->top_left != *compareTo;
	if (&location->top_right!= compareTo)
		unique = unique && location->top_right!= *compareTo;
	if (&location->bottom_left != compareTo)
		unique = unique && location->bottom_left!= *compareTo;
	if (&location->bottom_right != compareTo)
		unique = unique && location->bottom_right != *compareTo;
	return unique;
}

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_CELLS_H