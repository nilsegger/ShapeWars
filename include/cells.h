#ifndef INCLUDE_CELLS_H
#define INCLUDE_CELLS_H

#define MAP_WIDTH 100 // 100m
#define MAP_HEIGHT 100// 1km

#define CELL_WIDTH 20 
#define CELL_HEIGHT 20 

#define CELL_ROWS (MAP_HEIGHT / CELL_HEIGHT)
#define CELL_COLS (MAP_WIDTH / CELL_WIDTH)

// Should be max 65536
#define CELLS_COUNT MAP_WIDTH / CELL_WIDTH * MAP_HEIGHT / CELL_HEIGHT

void draw_cell(int index);
void initialise_cells(ecs_world_t* world, ecs_entity_t* cells);
void cells_begin_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity);
void cells_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity, Position* position, Size* size, CellLocation* location);
Position cell_position(int index);
int cell_row(int index);
int cell_col(int index);
int cell_row_first(int row);
int cell_row_last(int row);
int cell_single_index(Position* position);
int cell_neighbour_top_left(int index);
int cell_neighbour_top(int index);
int cell_neighbour_top_right(int index);
int cell_neighbour_left(int index);
int cell_neighbour_right(int index);
int cell_neighbour_bottom_left(int index);
int cell_neighbour_bottom(int index);
int cell_neighbour_bottom_right(int index);

bool cell_tracker_top_right_unique(CellLocation* location);
bool cell_tracker_bottom_left_unique(CellLocation* location);
bool cell_tracker_bottom_right_unique(CellLocation* location);

/*
uint64_t create_cells_index(uint16_t first, uint16_t second, uint16_t third, uint16_t fourth);
uint16_t cell_from_cells_int(uint64_t cells_indices, int index);
*/

#endif // INCLUDE_CELLS_H