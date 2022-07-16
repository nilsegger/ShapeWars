#include <stdint.h>
#include <stdlib.h>

#ifndef INCLUDE_HELPERS_H
#define INCLUDE_HELPERS_H

#define SCREEN_RATIO (1334.0f / 750.0f)
#define SCREEN_WIDTH 500

#define WORLD_TO_SCREEN (SCREEN_WIDTH / 1000.0f)

#ifdef __cplusplus
extern "C" {
#endif

inline int random(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

#ifdef __cplusplus
}
#endif

#endif // INCLUDE_HELPERS_H
