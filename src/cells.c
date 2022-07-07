#include <flecs.h>
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

void initialise_cells(ecs_world_t* world, ecs_entity_t* cells) {
    ECS_TAG(world, Cell);

    for (int i = 0; i < CELLS_COUNT; i++) {
        ecs_entity_t cell = ecs_new_entity(world, 0);
        ecs_add_id(world, cell, Cell);
        cells[i] = cell;
    }
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
    return (int)(position->y / (float)CELL_HEIGHT) * CELL_COLS + (int)(position->x / (float)CELL_WIDTH);
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

void cells_begin_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity) {
	 ECS_COMPONENT(world, Position);
	 ECS_COMPONENT(world, Size);
	 ECS_COMPONENT(world, CellLocation);

	 Position* position = ecs_get(world, entity, Position);
     Size* size = ecs_get(world, entity, Size);
     CellLocation* location = ecs_get(world, entity, CellLocation);

     cells_track_entity(world, cells, entity, position, size, location);
}

void cells_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity, Position* position, Size* size, CellLocation* location) {
	ECS_COMPONENT(world, Position);
	 ECS_COMPONENT(world, Size);
	 ECS_COMPONENT(world, CellLocation);


	Position top_right = { position->x + size->x, position->y };
	Position bottom_right = { position->x + size->x, position->y + size->y };
	Position bottom_left = { position->x, position->y + size->y };

	int cell_index = cell_single_index(position);
		
	if (location != NULL) {
	    location->top_left = cell_single_index(position);
		 location->top_right = cell_single_index(&top_right);
		 location->bottom_left = cell_single_index(&bottom_left);
		 location->bottom_right = cell_single_index(&bottom_right);
	}
	else {
		 ecs_set(world, entity, CellLocation, { cell_single_index(position), cell_single_index(&top_right), cell_single_index(&bottom_left), cell_single_index(&bottom_right)});
	}
}

bool cell_tracker_top_right_unique(CellLocation* location) {
    return location->top_right != location->top_left;
}
bool cell_tracker_bottom_left_unique(CellLocation* location) {
    bool unique = location->bottom_left != location->top_left &&
        location->bottom_left != location->top_right;
    return unique;
}

bool cell_tracker_bottom_right_unique(CellLocation* location) {
	bool unique = location->bottom_right != location->top_left &&
        location->bottom_right != location->top_right &&
        location->bottom_right != location->bottom_left;
    return unique;

}


/*
uint64_t create_cells_index(uint16_t first, uint16_t second, uint16_t third, uint16_t fourth) {
    return (((uint64_t)first) << (3 * 16)) ||
			(((uint64_t)second) << (2 * 16)) ||
			(((uint64_t)third) << (16)) ||
			((uint64_t)fourth);
}

uint16_t cell_from_cells_int(uint64_t cells_indices, int index) {
    assert(index >= 0 && index <= 3);
	return cells_indices >> ((3 - index) * 16);
}
*/
