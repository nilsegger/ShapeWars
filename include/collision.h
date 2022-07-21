#ifndef INCLUDE_COLLISION_H
#define INCLUDE_COLLISION_H

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

	for (int i = 1; i < 3; i++) {
		float p1 = dot_product(axis, &r1[i]);
		min1 = min(min1, p1);
		min1 = min(min1, p1);

		float p2 = dot_product(axis, &r2[i]);
		min2 = min(min2, p2);
		min2 = min(min2, p2);
	}

	if (max1 > min2 && max2 > min1) {
		float distance = min(max2 - min1, max1 - min2);
		float d_squared = (distance / dot_product(axis, axis) + 1e-10);
		push->x = axis->x * d_squared;
		push->y = axis->y * d_squared;
		return false;
	}
	return false;
}

inline bool rectangle_rotated_collide(Position* p1, Size* s1, float r1, Position* p2, Size* s2, float r2) {

	Position rr1[4];
	rotate_rectangle(p1, s1, r1, &rr1[1], &rr1[2], rr1, &rr1[3]);
	Position rr2[4];
	rotate_rectangle(p2, s2, r2, &rr2[1], &rr2[2], rr2, &rr2[3]);

	for (int i = 0; i < 4; i++) {
		Position axis = { rr1[(i + 1) % 4].x - rr1[i].x, rr1[(i + 1)].y - rr1[i].y};
		orth(&axis);

		Position push_vector = {0,0};
		if (is_separating_axis_rectangles(&axis, rr1, rr2, &push_vector)) {
			return false;
		}
	}

	/*
	for (int i = 0; i < 4; i++) {
		Position axis = { rr2[(i + 1) % 4].x - rr2[i].x, rr2[(i + 1)].y - rr2[i].y};
		orth(&axis);

		Position push_vector = {0,0};
		if (is_separating_axis_rectangles(&axis, rr1, rr2, &push_vector)) {
			return false;
		}

	}
	*/

	return true;

	/*
	Position offset = {p1->x - p2->x, p1->y - p2->y};

	for (int i = 0; i < 4; i++) {
		Position axis = (Position){ -rr1[(i + 1) % 4].y - rr1[i].y, rr1[(i+1)%4].x - rr1[i].x};

		float magnitude = sqrtf((axis.x * axis.x) + (axis.y * axis.y));
		if (magnitude != 0.0f) {
			axis.x *= 1.0f / magnitude;
			axis.y *= 1.0f / magnitude;
		}

		float dmin1 = (axis.x * rr1[0].x) + (axis.y * rr1[0].y);
		float dmax1 = dmin1;

		float dmin2 = (axis.x * rr2[0].x) + (axis.y * rr2[0].y);
		float dmax2 = dmin2;

		for (int j = 1; j < 4; j++) {
			float dot1 = (axis.x * rr1[j].x) + (axis.y * rr1[j].y);
			dmin1 = min(dmin1, dot1);
			dmax1 = max(dmax1, dot1);

			float dot2 = (axis.x * rr2[j].x) + (axis.y * rr2[j].y);
			dmin2 = min(dmin2, dot2);
			dmax2 = max(dmax2, dot2);
		}

		float scalerOffset = (axis.x * offset.x) + (axis.y * offset.y);
		dmin1 += scalerOffset;
		dmax1 += scalerOffset;

		if (dmin1 - dmax2 > 0.0f || dmin2 - dmax1 > 0.0f) return false;
	}

	return true;
	*/	
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
		collision_item_t* temp = iter;
		iter = iter->next;
		free(temp);
	}
	collision->n = 0;
	collision->first = NULL;
	collision->last = NULL;
}

inline void add_collision(collision_t* collision, entity_id_t a, entity_id_t b) {
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
					add_collision(&world->collisions, entity, other);
					continue;
				}

				if(rectangle_rotated_collide(&world->positions[entity], &world->sizes[entity], world->rotations[entity], &world->positions[other], &world->sizes[other], world->rotations[other])) {
				/* TODO HERE SPECIAL COLLISION FOR RECTANGLE TO CIRCLE ETC MUST BE ADDED; EVEN ROTATED RECTS */
				// if (rectangle_collide(&world->positions[entity], &world->sizes[entity], &world->positions[other], &world->sizes[other])) {
					add_collision(&world->collisions, entity, other);
				}
			}
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_COLLISION_H
