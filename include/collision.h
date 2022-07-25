#ifndef INCLUDE_COLLISION_H
#define INCLUDE_COLLISION_H

#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>

#include "helpers.h"
#include "structs.h"
#include "cells.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	RECTANGLE_SIDE_ERROR = -1,
	RECTANGLE_LEFT,
	RECTANGLE_RIGHT,
	RECTANGLE_TOP,
	RECTANGLE_BOTTOM
} RectangleSide;

inline bool rectangle_collide(Position* p1, Size* s1, Position* p2, Size* s2) {
	return (p1->x < p2->x + s2->x
		&& p1->x + s1->x > p2->x
		&& p1->y < p2->y + s2->y
		&& p1->y + s1->y > p2->y);
}

inline bool is_separating_axis_rectangles(Position* axis, Position* r1, Position* r2, Position* push) {

	float min1 = dot_product(axis, r1);
	float max1 = min1;

	float min2 = dot_product(axis, r2);
	float max2 = min2;

	for (int i = 1; i < 4; i++) {
		float p1 = dot_product(axis, &r1[i]);
		min1 = MIN(min1, p1);
		max1 = MAX(max1, p1);

		float p2 = dot_product(axis, &r2[i]);
		min2 = MIN(min2, p2);
		max2 = MAX(max2, p2);
	}

	if (max1 >= min2 && max2 >= min1) {
		float distance = MIN(max2 - min1, max1 - min2);
		float d_squared = (distance / dot_product(axis, axis) + 1e-10f);
		push->x = axis->x * d_squared;
		push->y = axis->y * d_squared;
		return false;
	}
	return true;
}

inline bool rectangle_rotated_collide(Position* p1, Size* s1, float r1, Position* p2, Size* s2, float r2, Position* offset) {

	Position rr1[4];
	rotate_rectangle(p1, s1, r1, &rr1[1], &rr1[2], rr1, &rr1[3]);
	Position rr2[4];
	rotate_rectangle(p2, s2, r2, &rr2[1], &rr2[2], rr2, &rr2[3]);

	float mpv_dot = 0.0f;

	for (int i = 0; i < 4; i++) {
		Position axis = { rr1[(i + 1) % 4].x - rr1[i].x, rr1[(i + 1)].y - rr1[i].y};
		orth(&axis);

		Position push_vector = {0,0};
		if (is_separating_axis_rectangles(&axis, rr1, rr2, &push_vector)) {
			return false;
		}

		if (i == 0) {
			*offset = push_vector;
			mpv_dot = dot_product(offset, offset);
			continue;
		}
		
		float npd = dot_product(&push_vector, &push_vector);
		if (npd < mpv_dot) {
			*offset = push_vector;
			mpv_dot = npd;
		}
	}

	for (int i = 0; i < 4; i++) {
		Position axis = { rr2[(i + 1) % 4].x - rr2[i].x, rr2[(i + 1)].y - rr2[i].y};
		orth(&axis);

		Position push_vector = {0,0};
		if (is_separating_axis_rectangles(&axis, rr1, rr2, &push_vector)) {
			return false;
		}

		float npd = dot_product(&push_vector, &push_vector);
		if (npd < mpv_dot) {
			*offset = push_vector;
			mpv_dot = npd;
		}
	}

	Position center1 = { p1->x + (s1->x / 2.0f), p1->y + (s1->y / 2.0f)};
	Position center2 = { p2->x + (s2->x / 2.0f), p2->y + (s2->y / 2.0f)};
	Position c1c2 = { center2.x - center1.x, center2.y - center1.y };

	if (dot_product(offset, &c1c2) > 0) {
		offset->x = -offset->x;
		offset->y = -offset->y;
	}

	return true;
}

inline bool point_left(Position* point, Position* a, Position* b) {
	return (a->x - point->x) * (b->y - point->y) > (a->y - point->y) * (b->x - point->x);
}

inline RectangleSide side_of_point(Position* point, Position* rectangle, Size* size) {
	// Inspired by https://math.stackexchange.com/questions/194550/how-to-find-on-which-outer-side-of-the-rectangle-falls-the-point 
	Position center = { rectangle->x + (size->x / 2.0f), rectangle->y + (size->y / 2.0f) };
	float squared_distance = (point->x - center.x) * (point->x - center.x) + (point->y - center.y) * (point->y - center.y);
	Vector2 forward = { size->x * squared_distance, size->y * squared_distance };

	Position bl = { center.x - forward.x, center.y - forward.y };
	Position tr = { center.x + forward.x, center.y + forward.y };

	Position tl = { center.x - forward.x, center.y + forward.y };
	Position br = { center.x + forward.x, center.y - forward.y };

	RectangleSide result = RECTANGLE_SIDE_ERROR;

	if (point_left(point, &bl, &tr)) {
		if (point_left(point, &br, &tl))
			result = RECTANGLE_LEFT;
		else
			result = RECTANGLE_TOP;
	}
	else {
		if (point_left(point, &br, &tl))
			result = RECTANGLE_BOTTOM;
		else
			result = RECTANGLE_RIGHT;
	}

	return result;
}

inline void empty_collision(collision_t* collision) {
	collision_item_t* iter = collision->first;
	while (iter != NULL) {
		if (iter->offset != NULL) free(iter->offset);
		collision_item_t* temp = iter;
		iter = iter->next;
		free(temp);
	}
	collision->n = 0;
	collision->first = NULL;
	collision->last = NULL;
}

inline void add_collision(collision_t* collision, entity_id_t a, entity_id_t b, Position* offset) {
	if (collision->first == NULL) {
		collision->first = (collision_item_t*)calloc(1, sizeof(struct CollisionItem));

		if (collision->first == NULL) {
			fprintf(stderr, "Failed to allocated memory for CollisionItem\n");
			return;
		}

		collision->last = collision->first;
	}
	else {
		collision->last->next = (collision_item_t*)calloc(1, sizeof(struct CollisionItem));
		if (collision->last->next == NULL) {
			fprintf(stderr, "Failed to allocated memory for CollisionItem\n");
			return;
		}
		collision->last = collision->last->next;
	}
	collision->n++;
	collision->last->a = a;
	collision->last->b = b;
	collision->last->offset = offset;
}

inline bool common_lower_cell(world_t* world, uint16_t cell, entity_id_t a, entity_id_t b) {
	Location* la = &world->locations[a];
	Location* lb = &world->locations[b];

	if (la->bottom_left != - 1 && la->bottom_left < cell && cell_location_contains(lb, la->bottom_left)) return true;
	if (la->bottom_right != -1 && la->bottom_right < cell && cell_location_contains(lb, la->bottom_right)) return true;
	if (la->top_left != -1 && la->top_left < cell && cell_location_contains(lb, la->top_left)) return true;
	if (la->top_right != -1 && la->top_right < cell && cell_location_contains(lb, la->top_right)) return true;

	return false;
}

inline bool bounding_box_collide(world_t* world, entity_id_t a, entity_id_t b) {
	// 0: bl, 1: tr
	Position* bba = &world->bounding_box[a * 2];
	Position* bbb = &world->bounding_box[b * 2];

	return (bba[0].x < bbb[1].x
		&& bba[1].x > bbb[0].x
		&& bba[0].y < bbb[1].y
		&& bba[1].y > bbb[0].y);
}

inline void find_collisions(world_t* world) {

	empty_collision(&world->collisions);

	for (uint16_t i = 0; i < world->grid.count; i++) {
		cell_t* cell = &world->grid.cells[i];

		if (cell->count < 2) continue;

		for (uint16_t j = 0; j < cell->count - 1; j++) {
			entity_id_t entity = cell->entites[j];
			for (uint16_t o = j + 1; o < cell->count; o++) {
				entity_id_t other = cell->entites[o];

				// Collision between these two entities must already have been checked in a cell with lower index
				if (common_lower_cell(world, i, entity, other)) continue;

				if (!bounding_box_collide(world, entity, other)) continue;

				if (world->rotations[entity] == 0.0f && world->rotations[other] == 0.0f) {
					add_collision(&world->collisions, entity, other, NULL);
					continue;
				}

				Position* offset = (Position*)calloc(1, sizeof(Position));
				if(rectangle_rotated_collide(&world->positions[entity], &world->sizes[entity], world->rotations[entity], &world->positions[other], &world->sizes[other], world->rotations[other], offset)) {
				/* TODO HERE SPECIAL COLLISION FOR RECTANGLE TO CIRCLE ETC MUST BE ADDED; EVEN ROTATED RECTS */
					add_collision(&world->collisions, entity, other, offset);
				}
			}
		}
	}
}


inline void find_closest_in_cell(world_t* world, cell_t* cell, entity_id_t entity, EntityType filter, entity_id_t* closest_entity, float* closest_distance) {

	for (uint16_t i = 0; i < cell->count; i++) {
		entity_id_t other = cell->entites[i];
		if (other == entity) continue;

		if (world->types[other] != filter) continue;

		if (*closest_entity == entity) {
			*closest_entity = other;
			*closest_distance = min_distance_between_rects_squared(&world->positions[entity], &world->sizes[entity], world->rotations[entity], &world->positions[other], &world->sizes[other], world->rotations[other]);
		}
		else {
			float cd = min_distance_between_rects_squared(&world->positions[entity], &world->sizes[entity], world->rotations[entity], &world->positions[other], &world->sizes[other], world->rotations[other]);
			if (cd < *closest_distance) {
				*closest_distance = cd;
				*closest_entity = other;
			}
		}
	}

}


inline bool find_closest(world_t* world, entity_id_t entity, EntityType filter, entity_id_t* closest_entity, float* closest_distance) {
	*closest_entity = entity;
	*closest_distance = 0;

	int row = cell_row(world, world->locations[entity].bottom_left);
	int col = cell_col(world, world->locations[entity].bottom_left);
	int row_size = 2;

	int iter = 1;

	while (iter > 0) {

		for (int i = 0; i < row_size; i++) {

			int c = col + i;
			if (c < 0 || c >= world->grid.cols) continue;

			//bottom
			if (row >= 0) {
				int index = row * world->grid.cols + c;
				find_closest_in_cell(world, &world->grid.cells[index], entity, filter, closest_entity, closest_distance);
			}

			//top
			if (row + row_size - 1 < world->grid.rows) {
				int index = (row + row_size - 1) * world->grid.cols + c;
				find_closest_in_cell(world, &world->grid.cells[index], entity, filter, closest_entity, closest_distance);
			}
		}

		for (int i = 0; i < row_size - 2; i++) {

			int r = row + 1 + i;
			if (r >= world->grid.rows) break;

			if (r < 0) continue;

			// left
			if (col >= 0) {
				int index = r * world->grid.cols + col;
				find_closest_in_cell(world, &world->grid.cells[index], entity, filter, closest_entity, closest_distance);
			}

			// right
			if (col + row_size < world->grid.cols) {
				int index = r * world->grid.cols + col + row_size;
				find_closest_in_cell(world, &world->grid.cells[index], entity, filter, closest_entity, closest_distance);
			}
		}

		row_size += 2;
		col--;
		row--;
		if (*closest_entity != entity) iter--;

		if (row < 0 && row + row_size >= world->grid.rows) break;
	}

	return *closest_entity != entity;

}

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_COLLISION_H
