#ifndef INCLUDE_LEVELS_H
#define INCLUDE_LEVELS_H

#include "structs.h"

#define LEVEL(name) world_t* name(Size screen)

world_t* debug_level(Size screen);

LEVEL(single_tower);



#endif // INCLUDE_LEVELS_H