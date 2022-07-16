
/*
    TODO
        - Use stack instead of heap
        - Remove WORLD_TO_SCREEN, use camera matrix

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"
#include "collision.h"

void initialise_window();
void display_fps();
void init_entities(world_t* world);
void draw_entity(world_t* world, entity_id_t entity);
void draw_number(int posX, int posY, int size, int n);

int main(void)
{
    Size map = { 50, 500 };
    Size cell = { 10, 50 };

    Camera2D camera = { .offset = {0.0f, SCREEN_WIDTH * SCREEN_RATIO}, .rotation = 0.0f, .target = {0.0f, 0.0f}, .zoom = 1.0f};
    initialise_window();

    const uint16_t entites = 1000;
    world_t* world = create_world(entites, map, cell, entites);

    init_entities(world);

    cells_begin_track_entites(world);

    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        DrawRectangle(0, 0, (int)(world->grid.map_size.x * WORLD_TO_SCREEN), (int)(world->grid.map_size.y * WORLD_TO_SCREEN), DARKBROWN);

        camera.offset.y += GetMouseWheelMove() * 10.0f;

        if (camera.offset.y < SCREEN_WIDTH * SCREEN_RATIO) camera.offset.y = SCREEN_WIDTH * SCREEN_RATIO;
        if (camera.offset.y > map.y * WORLD_TO_SCREEN) camera.offset.y = map.y * WORLD_TO_SCREEN;

        // move entity
        // Track entity
        //draw entity

        if (IsKeyDown(32) || IsKeyPressed(83)) {

			for (entity_id_t i = 0; i < world->entities_count; i++) {

                 Position ghost = { world->positions[i].x + (world->velocities[i].x * deltaTime), world->positions[i].y + (world->velocities[i].y * deltaTime) };
                bool didCollide = false;
                collision_t* collision = find_collision(world, i); //find_all_in_rect(&ghost, &world->sizes[i], cells, i, type, position, size, location);
				if (collision != NULL) {
					collision_item_t* iter = collision->first;
					while (iter != NULL) {
                        entity_id_t other = iter->id;
                        if (other == i) {
							iter = iter->next;
                            continue;
                        }

                        didCollide = true;

                        Position center = { world->positions[i].x + (world->sizes->x / 2.0f) , world->positions[i].y + (world->sizes->y / 2.0f)};
        
                        switch (side_of_point(&center, &world->positions[other], &world->sizes[other])) {
                        case RECTANGLE_LEFT:
                            world->positions[i].x = world->positions[other].x - world->sizes[i].x - 0.01f;
                            world->velocities[i].x = -(float)fabs(world->velocities[i].x);
                            world->velocities[other].x = (float)fabs(world->velocities[other].x);
                            break;
                        case RECTANGLE_RIGHT:
							world->positions[i].x = world->positions[other].x + world->sizes[other].x + 0.01f;
                            world->velocities[i].x = (float)fabs(world->velocities[i].x);
                            world->velocities[other].x = -(float)fabs(world->velocities[other].x);
                            break;
						case RECTANGLE_TOP:
							world->positions[i].y = world->positions[other].y + world->sizes[other].y + 0.01f;
                            world->velocities[i].y = (float)fabs(world->velocities[i].y);
                            world->velocities[other].x = -(float)fabs(world->velocities[other].x);
                            break;
						case RECTANGLE_BOTTOM:
							world->positions[i].y = world->positions[other].y - world->sizes[i].y - 0.01f;
                            world->velocities[i].y = -(float)fabs(world->velocities[i].y);
                            world->velocities[other].y = (float)fabs(world->velocities[other].y);
                            break;
                        }

						iter = iter->next;
					}
					collision_free(collision);
                }

                if (!didCollide) world->positions[i] = ghost;

                if (world->positions[i].x < 0.0f) {
                    world->positions[i].x = 0.0f;
                    world->velocities[i].x = (float)fabs(world->velocities[i].x);
                }

				if (world->positions[i].x > map.x - world->sizes[i].x) {
                    world->positions[i].x = map.x - world->sizes[i].x;
                    world->velocities[i].x = -(float)fabs(world->velocities[i].x);
                }

				if (world->positions[i].y < 0.0f) {
                    world->positions[i].y = 0.0f;
                    world->velocities[i].y = (float)fabs(world->velocities[i].y);
                }

				if (world->positions[i].y > map.y - world->sizes[i].y) {
                    world->positions[i].y = map.y - world->sizes[i].y;
                    world->velocities[i].y = -(float)fabs(world->velocities[i].y);
                }

                cells_track_entity(world, i);
            }
        }

		BeginMode2D(camera);
        for (entity_id_t i = 0; i < world->entities_count; i++) {
            draw_entity(world, i);
		}

		for (int i = 0; i < world->grid.count; i++) {
            cell_t* cell = &world->grid.cells[i];
            if (cell->count == 0) continue;
            Position cp = cell_position(world, i);
            unsigned char alpha = (int)(255.0f / (float)world->grid.max_entitites_per_cell* (float)cell->count);
            Color color = { 255, 0, 0, max(alpha, 50)};
            float y = (-cp.y - world->grid.cell_size.y);
            DrawRectangleLines((int)(cp.x * WORLD_TO_SCREEN), (int)(y * WORLD_TO_SCREEN), (int)(world->grid.cell_size.x* WORLD_TO_SCREEN), (int)(world->grid.cell_size.y * WORLD_TO_SCREEN), color);
        }

        EndMode2D();

        display_fps();
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
        
		world->velocities[i].x = (float)random(-30, 30);
        world->velocities[i].y = (float)random(-30, 30);

        world->colors[i].a = 255;
        world->colors[i].r = random(0, 255);
        world->colors[i].g = random(0, 255);
        world->colors[i].b = random(0, 255);
    };
}

void draw_entity(world_t* world, entity_id_t entity) {
    float y = -world->positions[entity].y - world->sizes[entity].y;
	DrawRectangle((int)(world->positions[entity].x * WORLD_TO_SCREEN), (int)(y * WORLD_TO_SCREEN), (int)(world->sizes[entity].x * WORLD_TO_SCREEN), (int)(world->sizes[entity].y * WORLD_TO_SCREEN), world->colors[entity]);
}

void initialise_window() {
	const int screenHeight = (int)(SCREEN_RATIO * SCREEN_WIDTH);
    InitWindow(SCREEN_WIDTH, screenHeight, "Window");
    SetTargetFPS(0);
}

void display_fps() {
	char fps[20];
	snprintf(fps, 20, "FPS: %d", GetFPS());
	DrawText(fps, 0, 0, 20, WHITE);
}

void draw_number(int posX, int posY,  int size, int n) {
	char number[20];
	snprintf(number, 20, "%d", n);
	DrawText(number, posX, posY, size, WHITE);

}