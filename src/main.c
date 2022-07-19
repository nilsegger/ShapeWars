
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"
#include "collision.h"
#include "drawing.h"

/*

    THE PLAN
    memory is saved like [troop1, troop2, ..., troopN, otherType1, otherType2, ..., otherTypeN, ..., inactiveWaitingBullet, inactiveWaitingBullet2, ... inactiveWaitingBulletN, PLACEHOLDER, PLACEHOLER]

    During gameplay, no new memory is allocated, memory has to be preallocated.
    When a building is placed, a the first placeholder entity is replaced with a building.

*/

void init_entities(world_t* world);

void doPhysics(world_t* world, float deltaTime) {
	for (entity_id_t i = 0; i < world->entities_count; i++) {

		world->positions[i].x += world->velocities[i].x * deltaTime;
		world->positions[i].y += world->velocities[i].y * deltaTime;

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

		Position center = { world->positions[iter->a].x + (world->sizes[iter->a].x / 2.0f) , world->positions[iter->a].y + (world->sizes[iter->a].y / 2.0f)};
		switch (side_of_point(&center, &world->positions[iter->b], &world->sizes[iter->b])) {
			case RECTANGLE_LEFT:
				world->positions[iter->a].x = world->positions[iter->b].x - world->sizes[iter->a].x - 0.01f;
				world->velocities[iter->a].x = -(float)fabs(world->velocities[iter->a].x);
				world->velocities[iter->b].x = (float)fabs(world->velocities[iter->b].x);
				break;
			case RECTANGLE_RIGHT:
				world->positions[iter->a].x = world->positions[iter->b].x + world->sizes[iter->b].x + 0.01f;
				world->velocities[iter->a].x = (float)fabs(world->velocities[iter->a].x);
				world->velocities[iter->b].x = -(float)fabs(world->velocities[iter->b].x);
				break;
			case RECTANGLE_TOP:
				world->positions[iter->a].y = world->positions[iter->b].y + world->sizes[iter->b].y + 0.01f;
				world->velocities[iter->a].y = (float)fabs(world->velocities[iter->a].y);
				world->velocities[iter->b].x = -(float)fabs(world->velocities[iter->b].x);
				break;
			case RECTANGLE_BOTTOM:
				world->positions[iter->a].y = world->positions[iter->b].y - world->sizes[iter->a].y - 0.01f;
				world->velocities[iter->a].y = -(float)fabs(world->velocities[iter->a].y);
				world->velocities[iter->b].y = (float)fabs(world->velocities[iter->b].y);
				break;
		}

		iter = iter->next;
	}
}

int main(void)
{
	
	const float physics_delta_time = 1.0f / 60.0f;
	double physics_timer_last = GetTime();

    const float aspect_ratio = 1334.0f / 750.0f;
    Size screen = { 500.0f, 500.0f * aspect_ratio };

    Size map = { 50, 500 };
    Size cell = { 10, 50 };

	const uint16_t entites = 1000;
    world_t* world = create_world(entites, map, cell, screen, entites);
    init_entities(world);
    cells_begin_track_entites(world);

	InitWindow(screen.x, screen.y, "Window");
    SetTargetFPS(0);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        DrawRectangle(0, 0, (int)(world->grid.map_size.x * world->to_screen_space), (int)(world->grid.map_size.y * world->to_screen_space), DARKBROWN);

        world->camera.offset.y += GetMouseWheelMove() * 10.0f;
        world->camera.offset.y = min(map.y * world->to_screen_space, world->camera.offset.y);
        world->camera.offset.y = max(screen.y, world->camera.offset.y);

        if (IsKeyDown(32) || IsKeyPressed(83)) {
			if (GetTime() - physics_timer_last >= physics_delta_time) {
				doPhysics(world, physics_delta_time);
				physics_timer_last = GetTime();
			}
        }

		BeginMode2D(world->camera);

		for (int i = 0; i < world->grid.count; i++) {
            cell_t* cell = &world->grid.cells[i];
            if (cell->count == 0) continue;
            Position cp = cell_position(world, i);
            unsigned char alpha = (int)(255.0f / (float)world->grid.max_entitites_per_cell* (float)cell->count);
            Color color = { 255, 0, 0, max(alpha, 50)};
            float y = (-cp.y - world->grid.cell_size.y);
            DrawRectangleLines((int)(cp.x * world->to_screen_space), (int)(y * world->to_screen_space), (int)(world->grid.cell_size.x* world->to_screen_space), (int)(world->grid.cell_size.y * world->to_screen_space), color);
        }

        for (entity_id_t i = 0; i < world->entities_count; i++) {
            draw_entity(world, i);
		}

		collision_item_t* iter = world->collisions.first;
		while (iter != NULL) {
			Size s = { 1.0f, 1.0f };
			draw_rect(world, &world->positions[iter->a], &s, RAYWHITE);
			draw_rect(world, &world->positions[iter->b], &s, RAYWHITE);
			iter = iter->next;
		}

        EndMode2D();

		DrawFPS(0, 0);
        EndDrawing();
    }

    free_world(world);
    CloseWindow();
    return 0;
}

void init_entities(world_t* world) {
    for (int i = 0; i < world->entities_count; i++) {
        world->types[i] = Walks;

		world->sizes[i].x = (float)random(1, 2);
        world->sizes[i].y = (float)random(1, 2);

        world->positions[i].x = (float)random(0, (int)(world->grid.map_size.x - 7.0f));
        world->positions[i].y = (float)random(0, (int)(world->grid.map_size.y - 7.0f));

        world->bounding_box[i * 2] = (Position){ world->positions[i].x, world->positions[i].y };
        world->bounding_box[i * 2 + 1] = (Position){ world->positions[i].x + world->sizes[i].x, world->positions[i].y + world->sizes[i].y};
        
		world->velocities[i].x = (float)random(-30, 30);
        world->velocities[i].y = (float)random(-30, 30);

        world->colors[i].a = 255;
        world->colors[i].r = random(0, 255);
        world->colors[i].g = random(0, 255);
        world->colors[i].b = random(0, 255);
    };
}
