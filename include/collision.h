#ifndef INCLUDE_COLLISION_H
#define INCLUDE_COLLISION_H

#include "helpers.h"
#include "structs.h"
#include "cells.h"

typedef enum  {
	left, right, top, bottom
} CollisionSide;

typedef struct CollisionItem {
	entity_id_t id;
	struct CollisionItem* next;
	CollisionSide side;
	Position point;
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

inline void add_collision(collision_t* collision, entity_id_t id, Position point, CollisionSide side) {
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
	collision->last->point = point;
	collision->last->side = side;
}

inline bool rectangle_collide(Position* p1, Size* s1, Position* p2, Size* s2) {
	return (p1->x < p2->x + s2->x
		&& p1->x + s1->x > p2->x
		&& p1->y < p2->y + s2->y
		&& p1->y + s1->y > p2->y);
}

inline void find_collision_in_cell(cell_t* cell, entity_id_t entity, EntityType* type, Position* position, Size* size, collision_t* collision) {	
	if (cell->n == 1) return;
	for (int i = 0; i < cell->n; i++) {
		entity_id_t other = cell->entites[i];
		if (entity == other) continue;
		if (rectangle_collide(&position[entity], &size[entity], &position[other], &size[other])) {
			Position entityCenter = { (position[entity].x + (size[entity].x / 2.0f)),
										(position[entity].y + (size[entity].y / 2.0f))};

			Position otherCenter = { (position[other].x + (size[other].x / 2.0f)),
										(position[other].y + (size[other].y / 2.0f))};

			Vector2 depth = { entityCenter.x - otherCenter.x, (entityCenter.y - otherCenter.y)};
			Size avgSize = { (size[entity].x + size[other].x) / 2.0f, (size[entity].y + size[other].y) / 2.0f};
			Size cross = { avgSize.x * depth.x, avgSize.y * depth.y };
			Position point; 
			CollisionSide side;

			if (cross.x < cross.y) {
				if (cross.x > -cross.y) {
					// bottom 
					point.y = position[other].y - size[entity].y - 0.001f;
					point.x = position[entity].x;
					side = bottom;

				}
				else {
						// left
					point.x = position[other].x - size[entity].x - 0.001f;
					point.y = position[entity].y;
					side = left;
				}
			}
			else {
				if (cross.x > -cross.y) {
										// right
					point.x = position[other].x + size[other].x + 0.001f;
					point.y = position[entity].y;
					side = right;

				}
				else {
				// top
					point.y = position[other].y + size[other].y + 0.001f;
					point.x = position[entity].x;
					side = top;
				}
			}

			add_collision(collision, cell->entites[i], point, side);
		}
	}
}

inline collision_t* find_collision(cell_t* cells, entity_id_t entity, EntityType* type, Position* position, Size* size, Location* location) {
	collision_t* collision = (collision_t*)calloc(1, sizeof(collision_t));
	find_collision_in_cell(&cells[location[entity].bottom_left], entity, type, position, size, collision);
	if(location->bottom_right != -1)
		find_collision_in_cell(&cells[location[entity].bottom_right], entity, type, position, size, collision);
	if(location->top_left != -1)
		find_collision_in_cell(&cells[location[entity].top_left], entity, type, position, size, collision);
	if(location->top_right != -1)
		find_collision_in_cell(&cells[location[entity].top_right], entity, type, position, size, collision);

	if (collision->n == 0) {
		collision_free(collision);
		return NULL;
	}

	return collision;
}

#endif // INCLUDE_COLLISION_H
