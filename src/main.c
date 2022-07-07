#include <stdio.h>
#include "flecs.h"
#include "raylib.h"

#include "helpers.h"
#include "structs.h"
#include "cells.h"

void initialise_window();
void display_fps();

/*
    Works only if each troop is always inside maximum 4 cells


    Cells: Height + Width
    ROWS: MAP_HEIGHT / CELL_HEIGHT
    COLS: MAP_WIDTH / CELL_WIDTH

    CELL_ROW: floor(INDEX / COLS)
    CELL_COL_INDEX = index - (CELL_ROW * COLS);

    1. Layout basic grid n: MAP_WIDTH / CELL_WIDTH * MAP_HEIGHT / CELL_HEIGTH
    
    CELL_INDEX = floor(POSITION.Y / CELL_HEIGHT) + floor(POSITION.X  / CELL_WIDTH)


    CellLocation in which Cell each troop is, -1 is not in any
    Cell entity saves childs for iter

    All troops in cell? iterate children
    In which cell is troop? calculate index via position of all 4 corners
*/
/*
void Move(ecs_iter_t* it) {
    Position* p = ecs_term(it, Position, 1);
    Velocity* v = ecs_term(it, Velocity, 2);

    printf("delta: %f", it->delta_time);

    char* type_str = ecs_table_str(it->world, it->table);
    printf("Move entities with [%s]\n", type_str);
    ecs_os_free(type_str);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;
    }
}
*/

int main(void)
{

    /* Create the world */
    ecs_world_t* world = ecs_init();

    /* Register components */
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Size);
	ECS_COMPONENT(world, Velocity);

    ecs_entity_t cells[CELLS_COUNT];
	initialise_cells(world, cells);
    printf("Created %d cells.\n", CELLS_COUNT);

    ecs_entity_t test = ecs_new_entity(world, 0);
    ecs_set(world, test, Position, { 0, 0 });
    ecs_set(world, test, Size, { 1, 1 });
    ecs_set(world, test, Velocity, { 0, 5 });

    cells_track_entity(world, cells, test);

    int test_cell = 0;
    float change = 1.0f;

    /* Register system */
    // ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    initialise_window();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();
		ecs_progress(world, deltaTime);

        change -= deltaTime;
        if (change < 0) {
            change = 0.1f;
            test_cell += 1;
            test_cell %= CELLS_COUNT;
		}

        draw_cell(test_cell, RED);
        draw_cell(cell_neighbour_bottom(test_cell), ORANGE);
        draw_cell(cell_neighbour_bottom_left(test_cell), VIOLET);
        draw_cell(cell_neighbour_bottom_right(test_cell), GRAY);
		draw_cell(cell_neighbour_top(test_cell), ORANGE);
        draw_cell(cell_neighbour_top_left(test_cell), VIOLET);
        draw_cell(cell_neighbour_top_right(test_cell), GRAY);

		draw_cell(cell_neighbour_left(test_cell), VIOLET);
        draw_cell(cell_neighbour_right(test_cell), GRAY);


        display_fps();
        EndDrawing();
    }
    CloseWindow();
    return ecs_fini(world);
}


void initialise_window() {
	const int screenHeight = SCREEN_RATIO * SCREEN_WIDTH;
    InitWindow(SCREEN_WIDTH, screenHeight, "Window");
    SetTargetFPS(0);
}

void display_fps() {
	char fps[20];
	snprintf(fps, 20, "FPS: %d", GetFPS());
	DrawText(fps, 0, 0, 20, WHITE);
}