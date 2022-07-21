#ifndef INCLUDE_DRAWING_H
#define INCLUDE_DRAWING_H

#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif

	bool rect_in_camera_view(world_t* world, Position* p, Size* s);
	bool text_in_camera_view(world_t* world, Position* p, int size);
	bool circle_in_camera_view(world_t* world, Position* p, float radius);

	void draw_entity(world_t* world, entity_id_t entity);

	void draw_number(world_t* world, Position* position, int size, int n, Color color);
	void draw_rect(world_t* world, Position* position, Size* size, Color color);
	void draw_rect_lines(world_t* world, Position* position, Size* size, Color color);
	void draw_circle(world_t* world, Position* position, float radius, Color color);

#ifdef __cplusplus
}
#endif


#endif // INCLUDE_DRAWING_H