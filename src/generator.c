#include "generator.h"

///////////////
// Helper funcs

static uint32_t rotl32(uint32_t value, uint32_t rotation)
{
    return (value << rotation) | (value >> (32u - rotation));
}

/////////////
// Algorithms

uint32_t LCG69069(void *context)
{
    state_t *state = context;
    state->lcg = state->lcg * 69069 + 1u;

    return state->lcg;
}

void xoshiro_seed(state_t *state, const uint32_t seed[4])
{
    state->xoshiro[0] = seed[0];
    state->xoshiro[1] = seed[1];
    state->xoshiro[2] = seed[2];
    state->xoshiro[3] = seed[3];

    if ((state->xoshiro[0] |
         state->xoshiro[1] |
         state->xoshiro[2] |
         state->xoshiro[3]) == 0u) {
        state->xoshiro[0] = 1u;
    }
}

uint32_t Xoshiro128(void *context)
{
    state_t *state = (state_t *)context;
    uint32_t *seed_state = state->xoshiro;

    uint32_t result =
        rotl32(seed_state[1] * 5u, 7u) * 9u;

    uint32_t t = seed_state[1] << 9u;

    seed_state[2] ^= seed_state[0];
    seed_state[3] ^= seed_state[1];
    seed_state[1] ^= seed_state[2];
    seed_state[0] ^= seed_state[3];
    seed_state[2] ^= t;
    seed_state[3] = rotl32(seed_state[3], 11u);

    return result;
}