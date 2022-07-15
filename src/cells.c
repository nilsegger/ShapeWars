#include <stdio.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"

void draw_cell(int index, Color color) {
    if (index == -1) return;
	Position pos = cell_position(index);
    int v = 255.0f / (float)CELLS_COUNT * (float)(index + 1);
    Color col = { 255, v, v, 100};
	DrawRectangleLines(pos.x * WORLD_TO_SCREEN, pos.y * WORLD_TO_SCREEN, CELL_WIDTH * WORLD_TO_SCREEN, CELL_HEIGHT * WORLD_TO_SCREEN, col);
}

Position cell_position(int index) {
    Position p;
    p.x = cell_col(index) * (float)CELL_WIDTH;
    p.y = cell_row(index) * (float)CELL_HEIGHT;
    return p;
}

int cell_row(int index) {
    return index / CELL_COLS;
}

int cell_col(int index) {
    if (index < CELL_COLS) return index;
    return index - ((cell_row(index)) * CELL_COLS);
}

int cell_row_first(int row) {
    return row * CELL_COLS;
}

int cell_row_last(int row) {
    return cell_row_first(row) + CELL_COLS - 1;
}

int cell_single_index(Position* position) {

    if (position->x >= MAP_WIDTH) position->x = MAP_WIDTH - 0.001f;
    if (position->x <= 0.0f) position->x = 0.001f;
    if (position->y >= MAP_HEIGHT) position->y = MAP_HEIGHT - 0.001f;
    if (position->y <= 0.0f) position->y = 0.001f;

    int index = (int)(position->y / CELL_HEIGHT) * CELL_COLS + (int)(position->x / CELL_WIDTH);
    if (index > CELLS_COUNT - 1) {
        fprintf(stderr, "return Position out of cell range. %d %d\n", index, CELLS_COUNT - 1);
        return CELLS_COUNT - 1;
    }
    return index;
}

int cell_neighbour_top_left(int index) {
    if (index < CELL_COLS || index == cell_row_first(cell_row(index))) return -1;
    return index - CELL_COLS - 1;
}

int cell_neighbour_top(int index) {
    if (index < CELL_COLS) return -1;
    return index - CELL_COLS;
}

int cell_neighbour_top_right(int index) {
    if (index < CELL_COLS || index == cell_row_last(cell_row(index))) return -1;
    return index - CELL_COLS + 1;
}

int cell_neighbour_left(int index) {
    if (index == 0 || cell_row_first(cell_row(index)) == index)  return -1;
    return index - 1;
}

int cell_neighbour_right(int index) {
    if (index == CELLS_COUNT - 1 || cell_row_last(cell_row(index)) == index) return -1;
    return index + 1;
}

int cell_neighbour_bottom_left(int index) {
    if (cell_row(index) == CELL_ROWS - 1 || cell_row_first(cell_row(index)) == index) return -1;
    return index + CELL_COLS - 1;
}

int cell_neighbour_bottom(int index) {
    if (cell_row(index) == CELL_ROWS - 1) return -1;
    return index + CELL_COLS;
}

int cell_neighbour_bottom_right(int index) {
    if (cell_row(index) == CELL_ROWS - 1 || cell_row_last(cell_row(index)) == index) return -1;
    return index + CELL_COLS + 1;
}

inline void cells_calculate_corner_cell_index(Position* position, Size* size, Location* location) {
	Position br = {position->x + size->x, position->y};
	Position tl = {position->x, position->y + size->y};
	Position tr = {position->x + size->x, position->y + size->y};

	location->bottom_left = cell_single_index(position);

	location->bottom_right = cell_single_index(&br);
    if (location->bottom_right == location->bottom_left) location->bottom_right = -1;

	location->top_left = cell_single_index(&tl);
    if (location->top_left == location->bottom_left
        || location->top_left == location->bottom_right) location->top_left = -1;

	location->top_right = cell_single_index(&tr);
    if (location->top_right == location->bottom_left
		|| location->top_right== location->bottom_right
        || location->top_right == location->top_left) location->top_right = -1;
}

void cells_add_entity(cell_t* cell, entity_id_t entity) {
    if (cell->n == MAX_ENTITES_PER_CELL - 1) {
        fprintf(stderr, "Error too many entities per cell.\n");
        return;
    }
    cell->entites[cell->n] = entity;
    cell->n++;
}

void cells_remove_entity(cell_t* cell, entity_id_t entity) {

    if (cell->n > 0) {
		if (cell->entites[cell->n - 1] == entity) {
			cell->n--;
			return;
		}

		for (int i = 0; i < cell->n - 1; i++) {
			if (cell->entites[i] == entity) {
				cell->entites[i] = cell->entites[cell->n - 1];
				cell->n--;
				return;
			}
		}
    }
    fprintf(stderr, "Entity not in cell...\n");
}

void cells_begin_track_entity(cell_t* cells, entity_id_t entity, Position* position, Size* size, Location* location) {

    cells_calculate_corner_cell_index(position, size, location);

	cells_add_entity(&cells[location->bottom_left], entity);
    if(location->bottom_right != -1)
		cells_add_entity(&cells[location->bottom_right], entity);
    if(location->top_left != -1)
		cells_add_entity(&cells[location->top_left], entity);
    if(location->top_right != -1)
		cells_add_entity(&cells[location->top_right], entity);
	}

inline void cells_location_apply_change(cell_t* cells, entity_id_t entity, Location* current, int*c , Location* updated, int* u) {
    if (*c == *u) return;

	if (*u != -1 && !cell_location_contains(current, *u)) {
		cells_add_entity(&cells[*u], entity);
	}

	if (*c != -1 && !cell_location_contains(updated, *c)) {
		cells_remove_entity(&cells[*c], entity);
	}
}

void cells_track_entity(cell_t* cells, entity_id_t entity, Position* position, Size* size, Location* location) {

    // Wennd left ecke usegönd aber rechti dinne blibed gits en doppelti zählig 

    Location updated = {0,0,0,0};
    cells_calculate_corner_cell_index(position, size, &updated);

    cells_location_apply_change(cells, entity, location, &location->bottom_left, &updated, &updated.bottom_left);
    cells_location_apply_change(cells, entity, location, &location->bottom_right, &updated, &updated.bottom_right);
    cells_location_apply_change(cells, entity, location, &location->top_left, &updated, &updated.top_left);
    cells_location_apply_change(cells, entity, location, &location->top_right, &updated, &updated.top_right);

    /*

/
	if (updated.bottom_left != location->bottom_left) {
        if (cell_location_bottom_left_unique(&updated) && !cell_location_contains(location, updated.bottom_left)) {
            cells_add_entity(&cells[updated.bottom_left], entity);
        }
        if (cell_location_bottom_left_unique(location) && !cell_location_contains(&updated, location->bottom_left)) {
            cells_remove_entity(&cells[location->bottom_left], entity);
        }
    }


    if (updated.top_left != location->top_left) {
        if (!cell_location_contains(location, updated.top_left)) {
            cells_add_entity(&cells[updated.top_left], entity);
        }
        if (!cell_location_contains(&updated, location->top_left)) {
            cells_remove_entity(&cells[location->top_left], entity);
        }
    }
    
	if (updated.top_right != location->top_right) {
        if (cell_location_top_right_unique(&updated) && !cell_location_contains(location, updated.top_right)) {
            cells_add_entity(&cells[updated.top_right], entity);
        }
        if(cell_location_top_right_unique(location) && !cell_location_contains(&updated, location->top_right)) {
            cells_remove_entity(&cells[location->top_right], entity);
        }
    }

	

	if (updated.bottom_right != location->bottom_right) {
        if (cell_location_bottom_right_unique(&updated) && !cell_location_contains(location, updated.bottom_right)) {
            cells_add_entity(&cells[updated.bottom_right], entity);
        }
        if (cell_location_bottom_right_unique(location) && !cell_location_contains(&updated, location->bottom_right)) {
            cells_remove_entity(&cells[location->bottom_right], entity);
        }
    }
    */
	location->bottom_left = updated.bottom_left;
    location->bottom_right = updated.bottom_right;
    location->top_left = updated.top_left;
    location->top_right = updated.top_right;
}
