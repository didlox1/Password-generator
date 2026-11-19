#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    uint32_t lcg; // LCG state 
		uint32_t xoshiro[4];  // Xoshiro state
} state_t; // Think about naming convention

uint32_t LCG69069(void *context);

uint32_t Xoshiro128(void *context);
void xoshiro_seed(state_t *state, const uint32_t seed[4]);
