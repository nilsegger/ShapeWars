#include <stdio.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"

Position cell_position(world_t* world, int index) {
    Position p;
    p.x = cell_col(world, index) * (float)world->grid.cell_size.x;
    p.y = cell_row(world, index) * (float)world->grid.cell_size.y;
    return p;
}

int cell_row(world_t*world, int index) {
    return index / world->grid.cols;
}

int cell_col(world_t*world, int index) {
    if (index < world->grid.cols) return index;
    return index - ((cell_row(world, index)) * world->grid.cols);
}

int cell_row_first(world_t*world, int row) {
    return row * world->grid.cols;
}

int cell_row_last(world_t*world, int row) {
    return cell_row_first(world, row) + world->grid.cols - 1;
}

int cell_single_index(world_t* world, Position* position) {

    if (position->x >= world->grid.map_size.x) position->x = world->grid.map_size.x - 0.001f;
    if (position->x <= 0.0f) position->x = 0.001f;
    if (position->y >= world->grid.map_size.y) position->y = world->grid.map_size.y - 0.001f;
    if (position->y <= 0.0f) position->y = 0.001f;

    int index = (int)(position->y / world->grid.cell_size.y) * world->grid.cols + (int)(position->x / world->grid.cell_size.x);
    if (index > world->grid.count - 1) {
        fprintf(stderr, "return Position out of cell range. %f:%f\nMax allowed %f:%f\nCells %d/%d\ncell size %f:%f\nworld cols %d\n", position->x, position->y, world->grid.map_size.x, world->grid.map_size.y, index, world->grid.count - 1, world->grid.cell_size.x, world->grid.cell_size.y, world->grid.cols);
        return world->grid.count - 1;
    }
    return index;
}

int cell_neighbour_top_left(world_t* world, int index) {
    if (index < world->grid.cols || index == cell_row_first(world, cell_row(world, index))) return -1;
    return index - world->grid.cols - 1;
}

int cell_neighbour_top(world_t* world, int index) {
    if (index < world->grid.cols) return -1;
    return index - world->grid.cols;
}

int cell_neighbour_top_right(world_t* world, int index) {
    if (index < world->grid.cols || index == cell_row_last(world, cell_row(world, index))) return -1;
    return index - world->grid.cols + 1;
}

int cell_neighbour_left(world_t* world, int index) {
    if (index == 0 || cell_row_first(world, cell_row(world, index)) == index)  return -1;
    return index - 1;
}

int cell_neighbour_right(world_t* world, int index) {
    if (index == world->grid.count - 1 || cell_row_last(world, cell_row(world, index)) == index) return -1;
    return index + 1;
}

int cell_neighbour_bottom_left(world_t* world, int index) {
    if (cell_row(world, index) == world->grid.rows - 1 || cell_row_first(world, cell_row(world, index)) == index) return -1;
    return index + world->grid.cols - 1;
}

int cell_neighbour_bottom(world_t* world, int index) {
    if (cell_row(world, index) == world->grid.rows - 1) return -1;
    return index + world->grid.cols;
}

int cell_neighbour_bottom_right(world_t* world, int index) {
    if (cell_row(world, index) == world->grid.rows - 1 || cell_row_last(world, cell_row(world, index)) == index) return -1;
    return index + world->grid.cols + 1;
}

void cells_add_entity(world_t* world, cell_t* cell, entity_id_t entity) {
    if (cell->count  == world->grid.max_entitites_per_cell - 1) {
        fprintf(stderr, "Error too many entities per cell.\n");
        return;
    }
    cell->entites[cell->count] = entity;
    cell->count++;
}

void cells_remove_entity(cell_t* cell, entity_id_t entity) {

    if (cell->count > 0) {
		if (cell->entites[cell->count - 1] == entity) {
			cell->count--;
			return;
		}

		for (int i = 0; i < cell->count - 1; i++) {
			if (cell->entites[i] == entity) {
				cell->entites[i] = cell->entites[cell->count - 1];
				cell->count--;
				return;
			}
		}
    }
    fprintf(stderr, "Entity not in cell...\n");
}

void cells_remove_tracking(world_t* world, entity_id_t entity) {
    
    if (world->locations[entity].bottom_left != -1) cells_remove_entity(&world->grid.cells[world->locations[entity].bottom_left], entity);
    if (world->locations[entity].bottom_right != -1) cells_remove_entity(&world->grid.cells[world->locations[entity].bottom_right], entity);
    if (world->locations[entity].top_left != -1) cells_remove_entity(&world->grid.cells[world->locations[entity].top_left], entity);
    if (world->locations[entity].top_right != -1) cells_remove_entity(&world->grid.cells[world->locations[entity].top_right], entity);

    world->locations[entity].bottom_left = -1;
    world->locations[entity].bottom_right = -1;
    world->locations[entity].top_left = -1;
    world->locations[entity].top_right = -1;

}

void cells_begin_track_entites(world_t* world) {

    for (entity_id_t i = 0; i < world->entities_count; i++) {

        if (world->alive[i] == false) {
            world->locations[i].bottom_left = -1;
            world->locations[i].top_left = -1;
            world->locations[i].bottom_right = -1;
            world->locations[i].top_right = -1;
            continue;
        }

        if (world->sizes[i].x > world->grid.cell_size.x) fprintf(stderr, "entity is too big. Max width of cell.\n");
        if (world->sizes[i].y > world->grid.cell_size.y) fprintf(stderr, "entity is too big. Max height of cell.\n");
        
        Location* location = &world->locations[i];

        cells_rectangle_location(world, i, location);

        cells_add_entity(world, &world->grid.cells[location->bottom_left], i);
        if (location->bottom_right != -1)
            cells_add_entity(world, &world->grid.cells[location->bottom_right], i);
        if (location->top_left != -1)
            cells_add_entity(world, &world->grid.cells[location->top_left], i);
        if (location->top_right != -1)
            cells_add_entity(world, &world->grid.cells[location->top_right], i);
    }

}

inline void cells_location_apply_change(world_t* world, entity_id_t entity, Location* current, int*c , Location* updated, int* u) {
    if (*c == *u) return;

	if (*u != -1 && !cell_location_contains(current, *u)) {
		cells_add_entity(world, &world->grid.cells[*u], entity);
	}

	if (*c != -1 && !cell_location_contains(updated, *c)) {
		cells_remove_entity(&world->grid.cells[*c], entity);
	}
}

void cells_track_entity(world_t* world, entity_id_t entity) {
    Location updated = {0,0,0,0};
    cells_rectangle_location(world, entity, &updated);

    Location* location = &world->locations[entity];

    cells_location_apply_change(world, entity, location, &location->bottom_left, &updated, &updated.bottom_left);
    cells_location_apply_change(world, entity, location, &location->bottom_right, &updated, &updated.bottom_right);
    cells_location_apply_change(world, entity, location, &location->top_left, &updated, &updated.top_left);
    cells_location_apply_change(world, entity, location, &location->top_right, &updated, &updated.top_right);
 
	location->bottom_left = updated.bottom_left;
    location->bottom_right = updated.bottom_right;
    location->top_left = updated.top_left;
    location->top_right = updated.top_right;
}
