#include <stdlib.h>
#include <stdio.h>

#include "structs.h"

void init_grid(grid_t* grid, Size map_size, Size cell_size, uint16_t maxEntitiesPerCell) {
	grid->max_entitites_per_cell = maxEntitiesPerCell;
	grid->cell_size = cell_size;
	grid->map_size = map_size;
	grid->rows = map_size.y / cell_size.y;
	grid->cols = map_size.x / cell_size.x;
	grid->count = map_size.x / cell_size.x * map_size.y / cell_size.y;
	grid->cells = (cell_t*)calloc(grid->count, sizeof(struct Cell));

	if (grid->cells == NULL) {
		fprintf(stderr, "Failed to allocate memory for cells.\n");
		return;
	}

	for (int i = 0; i < grid->count; i++) {
		grid->cells[i].count = 0;
		grid->cells[i].entites = (entity_id_t*)calloc(maxEntitiesPerCell, sizeof(entity_id_t));
	}
}

void free_grid_components(grid_t* grid) {

	for (uint16_t i = 0; i < grid->count; i++) {
		free(grid->cells[i].entites);
	}

	free(grid->cells);
}

world_t* create_world(uint16_t entites, Size map_size, Size cell_size, Size screen, uint16_t maxEntitiesPerCell) {
	world_t* world = (world_t*)calloc(1, sizeof(struct World));

	if (world == NULL) return NULL;

	init_grid(&world->grid, map_size, cell_size, maxEntitiesPerCell);

	world->entities_count = entites;
	world->bounding_box = (Position*)calloc((size_t)entites * 2, sizeof(Position));
	world->positions = (Position*)calloc(entites, sizeof(Position));
	world->sizes = (Size*)calloc(entites, sizeof(Size));
	world->types = (EntityType*)calloc(entites, sizeof(EntityType));
	world->velocities = (Velocity*)calloc(entites, sizeof(Velocity));
	world->locations = (Location*)calloc(entites, sizeof(Location));
	world->colors = (Color*)calloc(entites, sizeof(Color));

	world->to_screen_space = screen.x / map_size.x;

	world->screen = screen;
	world->camera = (Camera2D){.offset = {0.0f, screen.y}, .rotation = 0.0f, .target = {0.0f, 0.0f}, .zoom = 1.0f};

	return world;
}

void free_world(world_t* world) {
	if (world == NULL) return;

	free_grid_components(&world->grid);

	free(world->positions);
	free(world->sizes);
	free(world->types);
	free(world->velocities);
	free(world->locations);
	free(world->colors);

	free(world);
}
