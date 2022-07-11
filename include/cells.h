#ifndef INCLUDE_CELLS_H
#define INCLUDE_CELLS_H

#define MAP_WIDTH 100 // 100m
#define MAP_HEIGHT 100// 1km

#define CELL_WIDTH 10 
#define CELL_HEIGHT 10 

#define CELL_ROWS (MAP_HEIGHT / CELL_HEIGHT)
#define CELL_COLS (MAP_WIDTH / CELL_WIDTH)

// Should be max 65536
#define CELLS_COUNT (MAP_WIDTH / CELL_WIDTH * MAP_HEIGHT / CELL_HEIGHT)

#define MAX_ENTITES_PER_CELL 5 

typedef struct {
	int n;
	int entites[MAX_ENTITES_PER_CELL];
} cell_t;

void draw_cell(int index);
void cells_begin_track_entity(cell_t* cells, entity_id_t entity, Position* position, Size* size, Location* location);
void cells_track_entity(cell_t* cells, entity_id_t entity, Position* position, Size* size, Location* location);
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

inline bool cell_location_top_right_unique(Location* location) {
	return location->top_right != location->top_left;
}

inline bool cell_location_bottom_left_unique(Location* location) {
	return location->bottom_left != location->top_left
		&& location->bottom_left != location->top_right;
}

inline bool cell_location_bottom_right_unique(Location* location) {
	return location->bottom_right != location->top_left
		&& location->bottom_right != location->top_right
		&& location->bottom_right != location->bottom_left;
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

#endif // INCLUDE_CELLS_H