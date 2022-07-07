#include "flecs.h"
#include "raylib.h"

#include "helpers.h"
#include "structs.h"
#include "cells.h"


void draw_cell(int index, Color color) {
    if (index == -1) return;
	Position pos = cell_position(index);
	DrawRectangleLines(pos.x * WORLD_TO_SCREEN, pos.y * WORLD_TO_SCREEN, CELL_WIDTH * WORLD_TO_SCREEN, CELL_HEIGHT * WORLD_TO_SCREEN, color);
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
    return (int)(position->y / (float)CELL_HEIGHT) + (int)(position->x / (float)CELL_WIDTH);
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

void cells_track_entity(ecs_world_t* world, ecs_entity_t* cells, ecs_entity_t entity) {
	 ECS_COMPONENT(world, Position);
	 ECS_COMPONENT(world, Size);
	 ECS_COMPONENT(world, CellLocation);

     Position* position = ecs_get(world, entity, Position);
     Size* size = ecs_get(world, entity, Size);
     CellLocation* location = ecs_get(world, entity, CellLocation);

     /*
     if (location == NULL) {
        int base_cell = 
     }*/
    // ecs_set(world, test, CellLocation, { -1, -1, -1, -1 });
}
