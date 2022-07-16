#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"
#include "collision.h"

#define ENTITES 100

void initialise_window(Camera2D* camera);
void display_fps();
void init_entities(EntityType* type, Position* position, Size* size, Velocity* velocity, Color* color);
void draw_entity(entity_id_t entity, Position* position, Size* size, Color* color);
void draw_number(int posX, int posY, int size, int n);

int main(void)
{
    Camera2D camera = { .offset = {0.0f, MAP_HEIGHT * WORLD_TO_SCREEN}, .rotation = 0.0f, .target = {0.0f, 0.0f}, .zoom = 1.0f};
    initialise_window(&camera);

    cell_t cells[CELLS_COUNT];
    for (int i = 0; i < CELLS_COUNT; i++) {
        cells[i].n = 0;
    }

    EntityType type[ENTITES];
    Position position[ENTITES];
    Size size[ENTITES];
    Velocity velocity[ENTITES];
    Color color[ENTITES];
    Location location[ENTITES];

    init_entities(type, position, size, velocity, color);
    /*position[0] = (Position){MAP_WIDTH - 9.0f,  0.0f};
    size[0] = (Size){8.0f, 8.0f};
    color[0] = RED;
    velocity[0] = (Velocity){-5.0f, 0.0f};

	position[1] = (Position){ MAP_WIDTH / 2.0f, 0.0f };
    size[1] = (Size){8.0f, 8.0f};
    color[1] = GREEN;
    velocity[1] = (Velocity){0.0f, 0.0f};*/

    for (int i = 0; i < ENTITES; i++) {
        cells_begin_track_entity(cells, i, &position[i], &size[i], &location[i]);
    }
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

        DrawRectangle(0, 0, MAP_WIDTH * WORLD_TO_SCREEN, MAP_HEIGHT * WORLD_TO_SCREEN, DARKBROWN);

        // move entity
        // Track entity
        //draw entity

        if (IsKeyDown(32) || IsKeyPressed(83)) {

			for (entity_id_t i = 0; i < ENTITES; i++) {

                Position ghost = { position[i].x + (velocity[i].x * deltaTime), position[i].y + (velocity[i].y * deltaTime) };
                bool didCollide = false;
                collision_t* collision = find_collision(cells, i, type, position, size, location); //find_all_in_rect(&ghost, &size[i], cells, i, type, position, size, location);
				if (collision != NULL) {
					collision_item_t* iter = collision->first;
					while (iter != NULL) {
                        entity_id_t other = iter->id;
                        if (other == i) {
							iter = iter->next;
                            continue;
                        }

                        didCollide = true;

                        Position center = { position[i].x + (size->x / 2.0f) , position[i].y + (size->y / 2.0f)};
        
                        switch (side_of_point(&center, &position[other], &size[other])) {
                        case RECTANGLE_LEFT:
                            position[i].x = position[other].x - size[i].x - 0.01f;
                            velocity[i].x = -abs(velocity[i].x);
                            velocity[other].x = abs(velocity[other].x);
                            break;
                        case RECTANGLE_RIGHT:
							position[i].x = position[other].x + size[other].x + 0.01f;
                            velocity[i].x = abs(velocity[i].x);
                            velocity[other].x = -abs(velocity[other].x);
                            break;
						case RECTANGLE_TOP:
							position[i].y = position[other].y + size[other].y + 0.01f;
                            velocity[i].y = abs(velocity[i].y);
                            velocity[other].x = -abs(velocity[other].x);
                            break;
						case RECTANGLE_BOTTOM:
							position[i].y = position[other].y - size[i].y - 0.01f;
                            velocity[i].y = -abs(velocity[i].y);
                            velocity[other].y = abs(velocity[other].y);
                            break;
                        }

						iter = iter->next;
					}
					collision_free(collision);
                }

                if (!didCollide) position[i] = ghost;

                if (position[i].x < 0.0f) {
                    position[i].x = 0.0f;
                    velocity[i].x = abs(velocity[i].x);
                }

				if (position[i].x > MAP_WIDTH - size[i].x) {
                    position[i].x = MAP_WIDTH - size[i].x;
                    velocity[i].x = -abs(velocity[i].x);
                }

				if (position[i].y < 0.0f) {
                    position[i].y = 0.0f;
                    velocity[i].y = abs(velocity[i].y);
                }

				if (position[i].y > MAP_HEIGHT - size[i].y) {
                    position[i].y = MAP_HEIGHT - size[i].y;
                    velocity[i].y = -abs(velocity[i].y);
                }

                cells_track_entity(cells, i, &position[i], &size[i], &location[i]);
            }
        }

		BeginMode2D(camera);
        for (int i = 0; i < ENTITES; i++) {
            draw_entity(i, position, size, color);
		}
		for (int i = 0; i < CELLS_COUNT; i++) {
            if (cells[i].n == 0) continue;
            Position cp = cell_position(i);
            int alpha = (int)(255.0f / (float)MAX_ENTITES_PER_CELL * (float)cells[i].n);
            Color color = { 255.0f, 0, 0, max(alpha, 50.0f)};
            float y = (-cp.y - CELL_HEIGHT);
            DrawRectangleLines(cp.x * WORLD_TO_SCREEN, y * WORLD_TO_SCREEN, CELL_WIDTH * WORLD_TO_SCREEN, CELL_HEIGHT * WORLD_TO_SCREEN, color);
            // draw_number(cp.x * WORLD_TO_SCREEN, y * WORLD_TO_SCREEN, 10, i);

            /*for (int j = 0; j < cells[i].n; j++) {
                draw_number(cp.x * WORLD_TO_SCREEN, y * WORLD_TO_SCREEN + (11 * j), 10, cells[i].entites[j]);
            }*/
        }

        EndMode2D();

        display_fps();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void init_entities(EntityType* type, Position* position, Size* size, Velocity* velocity, Color* color) {
    for (int i = 0; i < ENTITES; i++) {
        type[i] = Walks;

		size[i].x = random(2, 6);
        size[i].y = random(2, 6);

        position[i].x = random(0, MAP_WIDTH - size->x - 5);
        position[i].y = random(0, MAP_HEIGHT - size->y - 5);
        
		velocity[i].x = random(0, 30);
        velocity[i].y = random(0, 30);

        color[i].a = 255;
        color[i].r = random(0, 255);
        color[i].g = random(0, 255);
        color[i].b = random(0, 255);
    };
}

void draw_entity(entity_id_t entity, Position* position, Size* size, Color* color) {
    float y = -position[entity].y - size[entity].y;
	DrawRectangle(position[entity].x * WORLD_TO_SCREEN, y * WORLD_TO_SCREEN, size[entity].x * WORLD_TO_SCREEN, size[entity].y * WORLD_TO_SCREEN, color[entity]);
}

void initialise_window(Camera2D* camera) {
	const int screenHeight = (int)(SCREEN_RATIO * SCREEN_WIDTH);
    InitWindow(SCREEN_WIDTH, screenHeight, "Window");
    SetTargetFPS(0);
    BeginMode2D(*camera);
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