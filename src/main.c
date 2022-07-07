#include <stdio.h>
#include <stdlib.h>

#include <flecs.h>
#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"

void initialise_window();
void display_fps();
void create_random_entities(ecs_world_t* world, ecs_entity_t* cells, int n);

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

void Move(ecs_iter_t* it) {
    Position* p = ecs_term(it, Position, 1);
    Size* s = ecs_term(it, Size, 2);
    Velocity* v = ecs_term(it, Velocity, 3);
    CellLocation* l = ecs_term(it, CellLocation, 4);

    char* type_str = ecs_table_str(it->world, it->table);
    ecs_os_free(type_str);

    for (int i = 0; i < it->count; i++) {
        p[i].x += v[i].x * it->delta_time;
        p[i].y += v[i].y * it->delta_time;

        cells_track_entity(it->world, it->ctx, it->entities[i], &p[i], &s[i], &l[i]);

        bool change_x = p[i].x < 0.0f || p[i].x + s[i].x > MAP_WIDTH;
        if(change_x) {
            v[i].x *= -1;
        }
        bool change_y = p[i].y < 0.0f || p[i].y + s[i].y > MAP_HEIGHT;
		if(change_y) {
            v[i].y *= -1;
        }
	}
}

void DrawBoxes(ecs_iter_t* it) {
	Position* p = ecs_term(it, Position, 1);
    Size* s = ecs_term(it, Size, 2);
    CellLocation* l = ecs_term(it, CellLocation, 3);
    Color* c = ecs_term(it, Color, 4);

    char* type_str = ecs_table_str(it->world, it->table);
    ecs_os_free(type_str);

    for (int i = 0; i < it->count; i++) {
        DrawRectangle(p[i].x * WORLD_TO_SCREEN, p[i].y * WORLD_TO_SCREEN, s[i].x * WORLD_TO_SCREEN, s[i].y * WORLD_TO_SCREEN, c[i]);

        draw_cell(l[i].top_left);
        if(cell_tracker_top_right_unique(&l[i]))
			draw_cell(l[i].top_right);
        if(cell_tracker_bottom_left_unique(&l[i]))
			draw_cell(l[i].bottom_left);
        if(cell_tracker_bottom_right_unique(&l[i]))
			draw_cell(l[i].bottom_right);

	}
}

int main(void)
{

    /* Create the world */
    ecs_world_t* world = ecs_init();

    /* Register components */
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Size);
	ECS_COMPONENT(world, Velocity);
	ECS_COMPONENT(world, CellLocation);
	ECS_COMPONENT(world, Color);

    ecs_entity_t cells[CELLS_COUNT];
	initialise_cells(world, cells);
    printf("Created %d cells.\n", CELLS_COUNT);

        /* Register system */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Size, Velocity, CellLocation);
    ECS_SYSTEM(world, DrawBoxes, EcsPostUpdate, Position, Size, CellLocation, Color);

    ecs_system_init(world, &(ecs_system_desc_t) {
        .entity = { .name = "Move"},
            .ctx = cells
    });

    create_random_entities(world, cells, 1000);

    /*ecs_system_init(world, &(ecs_system_init_t) {
        .entity = {Move},
		.ctx = cells
    });*/

    initialise_window();
    
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        float deltaTime = GetFrameTime();

		ecs_progress(world, deltaTime);

        display_fps();
        EndDrawing();
    }
    CloseWindow();
    return ecs_fini(world);
}

int random(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void create_random_entities(ecs_world_t* world, ecs_entity_t* cells, int n) {
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Size);
	ECS_COMPONENT(world, Velocity);
	ECS_COMPONENT(world, CellLocation);
	ECS_COMPONENT(world, Color);
        
    for (int i = 0; i < n; i++) {
        srand(i);

        ecs_entity_t e = ecs_new_entity(world, 0);
        ecs_set(world, e, Position, { random(0, MAP_WIDTH), random(0, MAP_HEIGHT)});
        ecs_set(world, e, Size, { 5, 5 });
        ecs_set(world, e, Velocity, { random(0, 50), random(0, 50)});
        ecs_set(world, e, Color, { random(0, 255), random(0, 255), random(0, 255), 255});

        cells_begin_track_entity(world, cells, e);
    }

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