#ifndef INCLUDE_COLLISION_H
#define INCLUDE_COLLISION_H

#include "helpers.h"
#include "structs.h"
#include "cells.h"

typedef struct CollisionItem {
	entity_id_t id;
	struct CollisionItem* next;
} collision_item_t;

typedef struct {
	collision_item_t* first;
	collision_item_t* last;
	int n;
} collision_t;

inline void collision_free(collision_t* collision) {
	if (collision->n > 0) {
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
		collision->first = (collision_item_t*)calloc(1, sizeof(collision_item_t));
		collision->last = collision->first;
	}
	else {

		collision_item_t* iter = collision->first;
		while (iter != NULL) {
			if (iter->id == id) return;
			iter = iter->next;
		}

		collision->last->next = (collision_item_t*)calloc(1, sizeof(collision_item_t));
		collision->last = collision->last->next;
	}
	collision->n++;
	collision->last->id = id;
}

inline bool rectangle_collide(Position* p1, Size* s1, Position* p2, Size* s2) {
	return (p1->x < p2->x + s2->x)
		&& (p1->x + s1->x > p2->x)
		&& (p1->y < p2->y + s2->y)
		&& (p1->y + s1->y > p2->y);
}

inline void find_collision_in_cell(cell_t* cell, entity_id_t entity, EntityType* type, Position* position, Size* size, Location* location, collision_t* collision) {	
	for (int i = 0; i < cell->n; i++) {
		if (entity == cell->entites[i]) continue;
		if (rectangle_collide(&position[entity], &size[entity], &position[cell->entites[i]], &size[cell->entites[i]])) {
			add_collision(collision, cell->entites[i]);
		}
	}
}

inline collision_t* find_collision(cell_t* cells, entity_id_t entity, EntityType* type, Position* position, Size* size, Location* location) {
	collision_t* collision = (collision_t*)calloc(1, sizeof(collision_t));
	find_collision_in_cell(&cells[location->top_left], entity, type, position, size, location, collision);
	find_collision_in_cell(&cells[location->top_right], entity, type, position, size, location, collision);
	find_collision_in_cell(&cells[location->bottom_left], entity, type, position, size, location, collision);
	find_collision_in_cell(&cells[location->bottom_right], entity, type, position, size, location, collision);
	if (collision->n == 0) {
		collision_free(collision);
		return NULL;
	}
	return collision;
}

#endif // INCLUDE_COLLISION_H
