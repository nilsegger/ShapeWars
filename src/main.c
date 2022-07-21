
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"
#include "collision.h"
#include "drawing.h"
#include "levels.h"

/*

    THE PLAN
    memory is saved like [troop1, troop2, ..., troopN, otherType1, otherType2, ..., otherTypeN, ..., inactiveWaitingBullet, inactiveWaitingBullet2, ... inactiveWaitingBulletN, PLACEHOLDER, PLACEHOLER]

    During gameplay, no new memory is allocated, memory has to be preallocated.
    When a building is placed, a the first placeholder entity is replaced with a building.

*/

int main(void)
{
	
	const float physics_delta_time = 1.0f / 60.0f;
	double physics_timer_last = GetTime();

    const float aspect_ratio = 1334.0f / 750.0f;
    Size screen = { 500.0f, 500.0f * aspect_ratio };

    world_t* world = debug_level(screen);

	InitWindow((int)screen.x, (int)screen.y, "Window");
    SetTargetFPS(0);
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

		if (GetTime() - physics_timer_last >= physics_delta_time) {
			world->physics_func(world, physics_delta_time);
			physics_timer_last = GetTime();
		}

		BeginMode2D(world->camera);
		world->draw_func(world);
        EndMode2D();

		DrawFPS(0, 0);
        EndDrawing();
    }

    free_world(world);
    CloseWindow();
    return 0;
}


