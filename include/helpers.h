#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#ifndef INCLUDE_HELPERS_H
#define INCLUDE_HELPERS_H

#include "structs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEG_TO_RAD 0.0174533

inline int random(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

inline float dot_product(const Position* p1, const Position* p2) {
	return (p1->x * p2->x) + (p1->y * p2->y);
}

inline void orth(Position* p) {
	float tempx = p->x;
	p->x = -p->y;
	p->y = tempx;
}

inline Position rotate_point(Position point, Position* pivot, float angle) {
	float s = sinf(angle);
	float c = cosf(angle);

	point.x -= pivot->x;
	point.y -= pivot->y;

	return (Position){point.x * c - point.y * s + pivot->x, point.x * s + point.y * c + pivot->y};
}

inline void rotate_rectangle(Position* p, Size* s, float r, Position* bl, Position* br, Position* tl, Position* tr) {
	Position pivot = {p->x + (s->x / 2.0f), p->y + (s->y / 2.0f)};
	*bl = rotate_point(*p, &pivot, r);
	*br = rotate_point((Position) { p->x + s->x, p->y }, &pivot, r);
	*tl = rotate_point((Position) { p->x, p->y + s->y }, &pivot, r);
	*tr = rotate_point((Position) { p->x + s->x, p->y + s->y }, &pivot, r);
}


#ifdef __cplusplus
}
#endif

#endif // INCLUDE_HELPERS_H
