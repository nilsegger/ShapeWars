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
/*

inline Position rotate_point(Position point, float r) {
	return (Position){ point.x* cosf(r) - sinf(point.y), point.y* cosf(r) + point.y * sinf(r) };
}

inline void rotate_rectangle(Position* p1, Size* s1, float r1, Position* bl, Position* br, Position* tl, Position* tr) {
	*bl = rotate_point(*p1, r1);
	*br = rotate_point((Position) { p1->x + s1->x, p1->y }, r1);
	*tl = rotate_point((Position) { p1->x, p1->y + s1->y }, r1);
	*tr = rotate_point((Position) { p1->x + s1->x, p1->y + s1->y }, r1);

	
}

inline bool rectangle_rotated_collide(Position* p1, Size* s1, float r1, Position* p2, Size* s2, float r2) {

	Position ;


}
*/

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

typedef struct CollisionItem {
	entity_id_t id;
	struct CollisionItem* next;
} collision_item_t;

typedef struct Collision {
	collision_item_t* first;
	collision_item_t* last;
	int n;
} collision_t;

inline void collision_free(collision_t* collision) {
	if (collision->n > 0 && collision->first != NULL) {
		collision_item_t* iter = collision->first;
		while (iter != NULL) {
			collision_item_t* temp = iter;
			iter = iter->next;
			free(temp);
		}
	}
	free(collision);
}

inline void add_collision(collision_t* collision, entity_id_t id) {
	if (collision->first == NULL) {
		collision->first = (collision_item_t*)calloc(1, sizeof(struct CollisionItem));
		
		if (collision->first == NULL) {
			fprintf(stderr, "Failed to allocated memory for CollisionItem\n");
			return;
		}

		collision->last = collision->first;
	}
	else {

		collision_item_t* iter = collision->first;
		while (iter != NULL) {
			if (iter->id == id) return;
			iter = iter->next;
		}

		collision->last->next = (collision_item_t*)calloc(1, sizeof(struct CollisionItem));
		if (collision->last->next == NULL) {
			fprintf(stderr, "Failed to allocated memory for CollisionItem\n");
			return;
		}

		collision->last = collision->last->next;
	}
	collision->n++;
	collision->last->id = id;
}

inline void find_entity_collision_in_cell(world_t* world, cell_t* cell, entity_id_t entity, collision_t* collision) {	
	if (cell->count == 1) return;
	for (int i = 0; i < cell->count; i++) {
		entity_id_t other = cell->entites[i];
		if (entity == other) continue;
		if (rectangle_collide(&world->positions[entity], &world->sizes[entity], &world->positions[other], &world->sizes[other])) {
			add_collision(collision, cell->entites[i]);
		}
	}
}

inline collision_t* find_collision(world_t* world, entity_id_t entity) {
	collision_t* collision = (collision_t*)calloc(1, sizeof(struct Collision));

	if (collision == NULL) {
		fprintf(stderr, "Cannot allocate memory for collision.\n");
		return NULL;
	}

	Location location = world->locations[entity];

	find_entity_collision_in_cell(world, &world->grid.cells[location.bottom_left], entity, collision);
	if(location.bottom_right != -1)
		find_entity_collision_in_cell(world, &world->grid.cells[location.bottom_right], entity, collision);
	if(location.top_left != -1)
		find_entity_collision_in_cell(world, &world->grid.cells[location.top_left], entity, collision);
	if(location.top_right != -1)
		find_entity_collision_in_cell(world, &world->grid.cells[location.top_right], entity, collision);

	if (collision->n == 0) {
		collision_free(collision);
		return NULL;
	}

	return collision;
}

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_COLLISION_H
