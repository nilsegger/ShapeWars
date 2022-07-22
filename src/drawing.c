#include <stdio.h>

#include "drawing.h"
#include "helpers.h"

bool rect_in_camera_view(world_t* world, Position* p, Size* s) {
	return !(p->y * world->to_screen_space > world->camera.offset.y || (p->y + s->y) * world->to_screen_space < world->camera.offset.y - world->screen.y);
}

bool text_in_camera_view(world_t* world, Position* p, int size) {
	return !(p->y * world->to_screen_space > world->camera.offset.y || (p->y) * world->to_screen_space + size < world->camera.offset.y - world->screen.y);
}

bool circle_in_camera_view(world_t* world, Position* p, float radius) {
	return ((world->camera.offset.y - p->y * world->to_screen_space < radius || p->y - world->camera.offset.y - world->screen.y < radius));
}

void draw_rect(world_t* world, Position* position, Size* size, Color color) {
	if (rect_in_camera_view(world, position, size)) {
		float y = -position->y - size->y;
		DrawRectangle((int)(position->x * world->to_screen_space), (int)(y * world->to_screen_space), (int)(size->x * world->to_screen_space), (int)(size->y * world->to_screen_space), color);
	}
}

void draw_rect_lines(world_t* world, Position* position, Size* size, Color color) {
	if (rect_in_camera_view(world, position, size)) {
		float y = -position->y - size->y;
		DrawRectangleLines((int)(position->x * world->to_screen_space), (int)(y * world->to_screen_space), (int)(size->x * world->to_screen_space), (int)(size->y * world->to_screen_space), color);
	}
}

void draw_entity(world_t* world, entity_id_t entity) {
	if (rect_in_camera_view(world, &world->positions[entity], &world->sizes[entity])) {
		float y = -world->positions[entity].y - world->sizes[entity].y;
		if (world->rotations[entity] == 0.0f) {
			DrawRectangle((int)(world->positions[entity].x * world->to_screen_space), (int)(y * world->to_screen_space), (int)(world->sizes[entity].x * world->to_screen_space), (int)(world->sizes[entity].y * world->to_screen_space), world->colors[entity]);
		}
		else {
			Rectangle rect = { (world->positions[entity].x + world->sizes[entity].x / 2.0f) * world->to_screen_space , -(world->positions[entity].y + (world->sizes[entity].y / 2.0f)) * world->to_screen_space, world->sizes[entity].x * world->to_screen_space, world->sizes[entity].y * world->to_screen_space };
			DrawRectanglePro(rect, (Vector2) { rect.width / 2.0f, rect.height / 2.0f }, -world->rotations[entity], world->colors[entity]);

			/*
			Position pivot = (Position){world->positions[entity].x + (world->sizes[entity].x / 2.0f), world->positions[entity].y + (world->sizes[entity].y / 2.0f)};
			Position bottom_left = rotate_point(world->positions[entity], &pivot, world->rotations[entity] * DEG2RAD);
			draw_circle(world, &bottom_left, 4.0f, BLACK);

			Position top_right = rotate_point((Position) { world->positions[entity].x + world->sizes[entity].x, world->positions[entity].y + world->sizes[entity].y }, & pivot, world->rotations[entity] * DEG2RAD);
			draw_circle(world, &top_right, 4.0f, BLUE);
			*/

		}
	}
}

void draw_circle(world_t* world, Position* position, float radius, Color color) {
	if (circle_in_camera_view(world, position, radius)) {
		DrawCircle(position->x * world->to_screen_space, -position->y * world->to_screen_space, radius, color);
	}
}

void draw_number(world_t* world, Position* position, int size, int n, Color color) {
	if(text_in_camera_view(world, position, size)) {
		char number[20];
		snprintf(number, 20, "%d", n);
		DrawText(number, position->x * world->to_screen_space, -position->y * world->to_screen_space - size, size, color);
	}
}
