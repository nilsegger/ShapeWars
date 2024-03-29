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

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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

	Position result = { point.x * c - point.y * s + pivot->x, point.x * s + point.y * c + pivot->y };
	return result;
}

inline void rotate_rectangle(Position* p, Size* s, float r, Position* bl, Position* br, Position* tl, Position* tr) {

	if (r == 0.0f) {
		bl->x = p->x;
		bl->y = p->y;

		tl->x = p->x;
		tl->y = p->y + s->y;

		tr->x = p->x + s->x;
		tr->y = p->y + s->y;

		br->x = p->x + s->x;
		br->y = p->y;

		return;
	}

	Position pivot = {p->x + (s->x / 2.0f), p->y + (s->y / 2.0f)};
	*bl = rotate_point(*p, &pivot, r);
	Position brp = { p->x + s->x, p->y };
	*br = rotate_point(brp, &pivot, r);
	Position tlp = { p->x, p->y + s->y };
	*tl = rotate_point(tlp, &pivot, r);
	Position trp = { p->x + s->x, p->y + s->y };
	*tr = rotate_point(trp, &pivot, r);
}

inline float distance_squared(const Position* p1, const Position* p2) {
	Position offset = { p2->x - p1->x, p2->y - p1->y};
	return offset.x * offset.x + offset.y * offset.y;
}

inline float distance(const Position* p1, const Position* p2) {
	return sqrtf(distance_squared(p1, p2));
}

inline float distance_point_to_segment_squared(Position* p, Position* s1, Position* s2) {
	// taken from https://stackoverflow.com/a/1501725/7539501
	const float l2 = distance_squared(s1, s2);
	if (l2 == 0.0f) return distance_squared(p, s1);

	Position offset1 = { p->x - s1->x, p->y - s1->y };
	Position offset2 = { s2->x - s1->x, s2->y - s1->y };
	const float t = MAX(0.0f, MIN(1.0f, dot_product(&offset1, &offset2) / l2));

	const Position projection = { offset2.x * t + s1->x, offset2.y * t + s1->y };
	return distance_squared(p, &projection);
}

inline float distance_point_to_segment(Position* p, Position* s1, Position* s2) {
	return sqrtf(distance_point_to_segment_squared(p, s1, s2));
}

inline void min_distance_point_to_segment_squared(Position* p, Position* s1, Position* s2, float* min_distance) {
	float d = distance_point_to_segment_squared(p, s1, s2);
	if (d < *min_distance) *min_distance = d;
}

inline float min_distance_between_rects_squared(Position* p1, Size* s1, float r1, Position* p2, Size* s2, float r2) {
	// inspired by https://stackoverflow.com/a/3040703/7539501 and https://stackoverflow.com/a/1501725/7539501
	Position rectangle1[4];
	rotate_rectangle(p1, s1, r1, &rectangle1[1], &rectangle1[2], rectangle1, &rectangle1[3]);
	Position rectangle2[4];
	rotate_rectangle(p2, s2, r2, &rectangle2[1], &rectangle2[2], rectangle2, &rectangle2[3]);

	float min_distance = distance_point_to_segment_squared(&rectangle1[0], &rectangle2[0], &rectangle2[1]);
	min_distance_point_to_segment_squared(&rectangle1[0], &rectangle2[1], &rectangle2[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[0], &rectangle2[2], &rectangle2[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[0], &rectangle2[3], &rectangle2[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle1[1], &rectangle2[0], &rectangle2[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[1], &rectangle2[1], &rectangle2[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[1], &rectangle2[2], &rectangle2[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[1], &rectangle2[3], &rectangle2[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle1[2], &rectangle2[0], &rectangle2[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[2], &rectangle2[1], &rectangle2[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[2], &rectangle2[2], &rectangle2[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[2], &rectangle2[3], &rectangle2[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle1[3], &rectangle2[0], &rectangle2[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[3], &rectangle2[1], &rectangle2[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[3], &rectangle2[2], &rectangle2[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle1[3], &rectangle2[3], &rectangle2[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle2[0], &rectangle1[0], &rectangle1[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[0], &rectangle1[1], &rectangle1[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[0], &rectangle1[2], &rectangle1[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[0], &rectangle1[3], &rectangle1[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle2[1], &rectangle1[0], &rectangle1[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[1], &rectangle1[1], &rectangle1[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[1], &rectangle1[2], &rectangle1[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[1], &rectangle1[3], &rectangle1[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle2[2], &rectangle1[0], &rectangle1[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[2], &rectangle1[1], &rectangle1[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[2], &rectangle1[2], &rectangle1[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[2], &rectangle1[3], &rectangle1[0], &min_distance);

	min_distance_point_to_segment_squared(&rectangle2[3], &rectangle1[0], &rectangle1[1], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[3], &rectangle1[1], &rectangle1[2], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[3], &rectangle1[2], &rectangle1[3], &min_distance);
	min_distance_point_to_segment_squared(&rectangle2[3], &rectangle1[3], &rectangle1[0], &min_distance);

	return min_distance;
}


#ifdef __cplusplus
}
#endif

#endif // INCLUDE_HELPERS_H
