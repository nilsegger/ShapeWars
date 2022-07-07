#ifndef INCLUDE_CELLS_H
#define INCLUDE_CELLS_H

#define MAP_WIDTH 50 // 100m
#define MAP_HEIGHT 50 // 1km

#define CELL_WIDTH 5 
#define CELL_HEIGHT 5

#define CELL_ROWS (MAP_HEIGHT / CELL_HEIGHT)
#define CELL_COLS (MAP_WIDTH / CELL_WIDTH)

#define CELLS_COUNT MAP_WIDTH / CELL_WIDTH * MAP_HEIGHT / CELL_HEIGHT

void draw_cell(int index, Color color);
void initialise_cells(ecs_world_t* world, ecs_entity_t* cells);
void cells_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity);
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

#endif // INCLUDE_CELLS_H