#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static const char lowercase[] = "abcdefghijklmnopqrstuvwxyz";
static const char uppercase[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const char digits[] = "0123456789";
static const char safe_specials[] = "@#$%^&*";
static const char all_specials[] = "!@#$%^&*()-_=+[]{};:,.<>?";
    
typedef enum {
    SPECIAL_NONE,
    SPECIAL_SAFE,
    SPECIAL_ALL
} special_mode_t;

typedef struct {
    uint32_t length;
    bool lowercase;
    bool uppercase;
    bool digits;
    special_mode_t specials;
} password_profile_t;

/* A function returning a random uint32_t. */
typedef uint32_t (*random_u32_fn)(void *context);

bool password_generate(
    char *output,
    size_t output_capacity,
    const password_profile_t *profile,
    random_u32_fn random_u32,
    void *random_context
);