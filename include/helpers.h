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

	return (Position){point.x * c - point.y * s + pivot->x, point.x * s + point.y * c + pivot->y};
}

inline void rotate_rectangle(Position* p, Size* s, float r, Position* bl, Position* br, Position* tl, Position* tr) {
	Position pivot = {p->x + (s->x / 2.0f), p->y + (s->y / 2.0f)};
	*bl = rotate_point(*p, &pivot, r);
	*br = rotate_point((Position) { p->x + s->x, p->y }, &pivot, r);
	*tl = rotate_point((Position) { p->x, p->y + s->y }, &pivot, r);
	*tr = rotate_point((Position) { p->x + s->x, p->y + s->y }, &pivot, r);
}

inline float distance_squared(Position* p1, Position* p2) {
	Position offset = { p2->x - p1->x, p2->y - p1->y};
	return offset.x * offset.x + offset.y * offset.y;
}

inline float distance(Position* p1, Position* p2) {
	return sqrtf(distance_squared(p1, p2));
}

inline float distance_point_to_segment(Position* p, Position* s1, Position* s2) {
	const float l2 = distance_squared(s1, s2);
	if (l2 == 0.0f) return distance(p, s1);

	Position offset1 = { p->x - s1->x, p->y - s1->y };
	Position offset2 = { s2->x - s1->x, s2->y - s1->y };
	const float t = max(0.0f, min(1.0f, dot_product(&offset1, &offset2) / l2));

	const Position projection = { offset2.x * t + s1->x, offset2.y * t + s1->y };
	return distance(p, &projection);
}

inline float min_distance_between_rects(Position* p1, Size* s1, float r1, Position* p2, Size* s2, float r2) {
	// inspired by https://stackoverflow.com/a/3040703/7539501 and https://stackoverflow.com/a/1501725/7539501
	Position rectangle1[4];
	rotate_rectangle(p1, s1, r1, &rectangle1[1], &rectangle1[2], rectangle1, &rectangle1[3]);
	Position rectangle2[4];
	rotate_rectangle(p2, s2, r2, &rectangle2[1], &rectangle2[2], rectangle2, &rectangle2[3]);

	// Points of rectangles with lowest distance between each other
	// min_index[0] < min_index[1]
	int min_index1[2] = {0, 0};
	int min_index2[2] = {0, 1};
	float d[2] = {distance_squared(&rectangle1[0], &rectangle2[0]), distance_squared(&rectangle1[0], &rectangle2[1])};

	if (d[1] < d[0]) {
		min_index2[0] = 1;
		min_index2[1] = 0;
		float temp = d[0];
		d[0] = d[1];
		d[1] = temp;
	}

	for (int i = 0; i < 4; i++) {
		for (int j = (i == 0 ? 2 : 0); j < 4; j++) {
			float current_d = distance_squared(&rectangle1[i], &rectangle2[j]);
			if (current_d < d[0]) {
				min_index1[1] = min_index1[0];
				min_index2[1] = min_index2[0];
				d[1] = d[0];

				d[0] = current_d;
				min_index1[0] = i;
				min_index2[0] = j;
			}
			else if (current_d < d[1]) {
				d[1] = current_d;
				min_index1[1] = i;
				min_index2[1] = j;
			}
		}
	}

	Position line_segment[2];
	Position corner;

	if (min_index1[0] == min_index1[1]) {
		corner = rectangle1[min_index1[0]];
		line_segment[0] = rectangle2[min_index2[0]];
		line_segment[1] = rectangle2[min_index2[1]];
	}
	else {
		corner = rectangle1[min_index2[0]];
		line_segment[0] = rectangle1[min_index1[0]];
		line_segment[1] = rectangle1[min_index1[1]];
	}

	return distance_point_to_segment(&corner, line_segment, &line_segment[1]);
}


#ifdef __cplusplus
}
#endif

#endif // INCLUDE_HELPERS_H
