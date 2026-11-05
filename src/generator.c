#include "generator.h"

uint32_t simple_random(void *context)
{
    simple_rng_t *rng = context;
		// TODO: Later need to replace it with Xoshito algorithm
		// Right now LCG (69069 LCG) mod = 2^32 
    rng->state = rng->state * 69069 + 1u;

    return rng->state;
}