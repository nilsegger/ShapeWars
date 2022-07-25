#include "levels.h"

#include "helpers.h"
#include "cells.h"
#include "collision.h"
#include "drawing.h"

inline void add_rectangle_entity(world_t* world, entity_id_t entity, EntityType type, Position position, Size size, bool alive, Color color) {
	world->types[entity] = type;
	world->sizes[entity] = size;
	world->positions[entity] = position;
	world->alive[entity] = alive;
	world->bounding_box[entity * 2] = position;
	world->bounding_box[entity * 2 + 1] = (Position){position.x + size.x, position.y + size.y};
	world->colors[entity] = color;
	world->rotations[entity] = 0.0f;
}

void debugDrawFunc(world_t* world) {
	DrawRectangle(0, 0, (int)(world->grid.map_size.x * world->to_screen_space), (int)(world->grid.map_size.y * world->to_screen_space), DARKBROWN);
	world->camera.offset.y += GetMouseWheelMove() * 10.0f;
	world->camera.offset.y = min(world->grid.map_size.y * world->to_screen_space, world->camera.offset.y);
	world->camera.offset.y = max(world->screen.y, world->camera.offset.y);

	for (int i = 0; i < world->grid.count; i++) {
		cell_t* cell = &world->grid.cells[i];
		if (cell->count == 0) continue;
		Position cp = cell_position(world, i);
		unsigned char alpha = (int)(255.0f / (float)world->grid.max_entitites_per_cell* (float)cell->count);
		Color color = { 255, 0, 0, max(alpha, 50)};
		draw_rect_lines(world, &cp, &world->grid.cell_size, color);
	}

	for (entity_id_t i = 0; i < world->entities_count; i++) {
		if (world->alive[i] == false) continue;
		draw_entity(world, i);
	}

	collision_item_t* iter = world->collisions.first;
	while (iter != NULL) {
		Size s = { 1.0f, 1.0f };
		draw_rect(world, &world->positions[iter->a], &s, RAYWHITE);
		draw_rect(world, &world->positions[iter->b], &s, RAYWHITE);
		iter = iter->next;
	}
}

void debugPhysics(world_t* world, float deltaTime) {
	if (IsKeyDown(32) || IsKeyPressed(83)) {
		for (entity_id_t i = 0; i < world->entities_count; i++) {

			if (world->alive[i] == false) continue;

			world->positions[i].x += world->velocities[i].x * deltaTime;
			world->positions[i].y += world->velocities[i].y * deltaTime;
			world->rotations[i] += 45.0f * deltaTime;

			if (world->positions[i].x < 0.0f) {
				world->positions[i].x = 0.0f;
				world->velocities[i].x = (float)fabs(world->velocities[i].x);
			}

			if (world->positions[i].x > world->grid.map_size.x - world->sizes[i].x) {
				world->positions[i].x = world->grid.map_size.x - world->sizes[i].x;
				world->velocities[i].x = -(float)fabs(world->velocities[i].x);
			}

			if (world->positions[i].y < 0.0f) {
				world->positions[i].y = 0.0f;
				world->velocities[i].y = (float)fabs(world->velocities[i].y);
			}

			if (world->positions[i].y > world->grid.map_size.y - world->sizes[i].y) {
				world->positions[i].y = world->grid.map_size.y - world->sizes[i].y;
				world->velocities[i].y = -(float)fabs(world->velocities[i].y);
			}

			world->bounding_box[i * 2] = world->positions[i];
			world->bounding_box[i * 2 + 1] = (Position){ world->positions[i].x + world->sizes[i].x, world->positions[i].y + world->sizes[i].y };

			cells_track_entity(world, i);
		}

		find_collisions(world);

		collision_item_t* iter = world->collisions.first;

		while (iter != NULL) {

			if (iter->offset != NULL) {
				world->positions[iter->a].x += iter->offset->x / 2.0f;
				world->positions[iter->a].y += iter->offset->y / 2.0f;

				world->positions[iter->b].x -= iter->offset->x / 2.0f;
				world->positions[iter->b].y -= iter->offset->y / 2.0f;
			}

			iter = iter->next;
		}
	}
}

world_t* debug_level(Size screen) {
	Size map = { 50, 500 };
    Size cell = { 10, 50 };

	const uint16_t entites = 1000;
    world_t* world = create_world(entites, map, cell, screen, entites);

	world->physics_func = debugPhysics;
	world->draw_func = debugDrawFunc;

	for (int i = 0; i < world->entities_count; i++) {
		world->types[i] = ENTITY_NONE;

		world->sizes[i].x = (float)random(1, 2);
		world->sizes[i].y = (float)random(1, 2);

		world->positions[i].x = (float)random(0, (int)(world->grid.map_size.x - 7.0f));
		world->positions[i].y = (float)random(0, (int)(world->grid.map_size.y - 7.0f));

		world->bounding_box[i * 2] = (Position){ world->positions[i].x, world->positions[i].y };
		world->bounding_box[i * 2 + 1] = (Position){ world->positions[i].x + world->sizes[i].x, world->positions[i].y + world->sizes[i].y };

		world->velocities[i].x = (float)random(-30, 30);
		world->velocities[i].y = (float)random(-30, 30);

		world->colors[i].a = 255;
		world->colors[i].r = random(0, 255);
		world->colors[i].g = random(0, 255);
		world->colors[i].b = random(0, 255);

		world->rotations[i] = (float)random(0, 360);

		world->alive[i] = true;
	}

	return world;
}

void draw_single_tower(world_t* world) {

	world->camera.offset.y += GetMouseWheelMove() * 10.0f;
	world->camera.offset.y = min(world->grid.map_size.y * world->to_screen_space, world->camera.offset.y);
	world->camera.offset.y = max(world->screen.y, world->camera.offset.y);

	const Position worldOrigin = { 0.f, 0.f };
	draw_rect(world, &worldOrigin, &world->grid.map_size, RAYWHITE);

	for (entity_id_t i = 0; i < world->entities_count; i++) {
		if (world->alive[i] == false) continue;
		draw_entity(world, i);

		if (world->types[i] == ENTITY_TOWER) {
			tower_t* t = (tower_t*)world->entity_specific[i];
			draw_number(world, &world->positions[i], 20.0f, t->cooldown, GRAY);
		}
	}

	draw_circle(world, &world->positions[world->level_data], 5.0f, GREEN);
}

inline void spawn_tower_shot(world_t* world, Position positon) {
	for (entity_id_t i = 0; i < world->entities_count; i++) {
		if (world->alive[i] == false && world->types[i] == ENTITY_TOWER_SHOT) {
			world->alive[i] = true;
			world->positions[i] = positon;
			cells_track_entity(world, i);
			return;
		}
	}

	fprintf(stderr, "Unable to spawn bullet because there are not enough placeholders\n");
}

void update_single_tower(world_t* world, float deltaTime) {

	if (!IsKeyDown(32) && !IsKeyPressed(83)) return;

	for (entity_id_t i = 0; i < world->entities_count; i++) {
		if (world->alive[i] == false) continue;

		if (world->types[i] == ENTITY_TOWER) {
			tower_t* t = (tower_t*)world->entity_specific[i];
			t->cooldown -= deltaTime;

			if (t->cooldown <= 0.0f) {
				t->cooldown = 0.1f;

				entity_id_t closest;
				float closest_distance;
				if (find_closest(world, 0, ENTITY_ZOMBIE, &closest, &closest_distance)) {
					spawn_tower_shot(world, (Position) { world->positions[closest].x, 10.0f });
					world->level_data = closest;
				}
				else {
					world->level_data = 0;
				}

			}
		}
		else if (world->types[i] == ENTITY_TOWER_SHOT) {
			world->positions[i].y += world->velocities[i].y * deltaTime;

			world->bounding_box[i * 2] = world->positions[i];
			world->bounding_box[i * 2 + 1] = (Position){world->positions[i].x + world->sizes[i].x, world->positions[i].y + world->sizes[i].y};

			if (world->positions[i].y + world->sizes[i].y >= world->grid.map_size.y) {
				world->alive[i] = false;
				cells_remove_tracking(world, i);
			}
			else {
				cells_track_entity(world, i);
			}
		}
		else if (world->types[i] == ENTITY_ZOMBIE) {
			world->positions[i].y -= world->velocities[i].y * deltaTime;

			world->bounding_box[i * 2] = world->positions[i];
			world->bounding_box[i * 2 + 1] = (Position){world->positions[i].x + world->sizes[i].x, world->positions[i].y + world->sizes[i].y};

			if (world->positions[i].y > 0.0f) {
				cells_track_entity(world, i);
			}
			else {
				world->alive[i] = false;
				cells_remove_tracking(world, i);
			}
		}
	}
	
	find_collisions(world);
	collision_item_t* iter = world->collisions.first;
	while (iter != NULL) {

		if (!((world->types[iter->a] == ENTITY_TOWER_SHOT || world->types[iter->b] == ENTITY_TOWER_SHOT) && (world->types[iter->a] == ENTITY_ZOMBIE || world->types[iter->b] == ENTITY_ZOMBIE))) {
			iter = iter->next;
			continue;
		}

		entity_id_t towerShot = 0;
		entity_id_t zombie = 0;

		if (world->types[iter->a] == ENTITY_TOWER_SHOT) {
			towerShot = iter->a;
			zombie = iter->b;
		}
		else {
			towerShot = iter->b;
			zombie = iter->a;
		}

		world->alive[zombie] = false;
		cells_remove_tracking(world, zombie);

		iter = iter->next;
	}
}

LEVEL(single_tower) {
	Size map = {100.0f, 100.0f * (screen.y / screen.x) + 100.0f};
	Size cell = {map.x / 5.0f, map.y / 15.0f};

	const uint16_t towers = 1;
	const uint16_t maxTowerShots = 100;
	const uint16_t zombies  = 2000;
	const uint16_t entites = towers + maxTowerShots + zombies;

    world_t* world = create_world(entites, map, cell, screen, entites);
	world->draw_func = draw_single_tower;
	world->physics_func = update_single_tower;

	world->level_data = 0;

	/* TODO ADD CLEANUP FUNC */

	add_rectangle_entity(world, 0, ENTITY_TOWER, (Position) { map.x / 2.0f - 9.0f, 10.0f }, (Size) { 18.0f, 18.0f }, true, BLACK);

	world->entity_specific[0] = (tower_t*)calloc(1, sizeof(struct Tower));

	for (entity_id_t i = towers + 1; i < towers + maxTowerShots; i++) {
		add_rectangle_entity(world, i, ENTITY_TOWER_SHOT, (Position) { 0, 0 }, (Size) { 1.0f, 1.0f }, false, BLUE);
		world->velocities[i] = (Position){ 0.0f, 150.0f };
	}

	for (entity_id_t i = towers + maxTowerShots; i < towers + maxTowerShots + zombies; i++) {
		add_rectangle_entity(world, i, ENTITY_ZOMBIE, (Position) { random(0, map.x - 5), random(map.y / 2.0f, map.y - 5) }, (Size) { 5.0f, 5.0f }, true, RED);
		world->velocities[i] = (Position){ 0.0f, random(1, 8) };
		world->rotations[i] = (float)random(1, 360);
	}

	return world;
}
