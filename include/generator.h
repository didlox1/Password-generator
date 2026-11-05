#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    uint32_t state;
} simple_rng_t;

uint32_t simple_random(void *context);