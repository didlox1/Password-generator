#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

uint16_t HAL_ADC_ReadValue() {
    // Zwracamy losową wartość 12-bitową udającą szum pinu
    return rand() & 0x0FFF; 
}


uint32_t generate_adc_seed() {
    uint32_t seed = 0;
    for(int i = 0; i < 32; i++) {
        uint16_t adc_val = HAL_ADC_ReadValue();
        uint8_t noise_bit = adc_val & 0x01; // Ekstrakcja LSB
        seed = (seed << 1) | noise_bit;
    }
    return seed;
}

// Xoshiro128** 
static uint32_t prng_state[4];

static inline uint32_t rotl(const uint32_t x, int k) {
    return (x << k) | (x >> (32 - k));
}

void init_prng() {
    prng_state[0] = generate_adc_seed();
    prng_state[1] = generate_adc_seed();
    prng_state[2] = generate_adc_seed();
    prng_state[3] = generate_adc_seed();
    
    // Ochrona przed stanem zerowym
    if(prng_state[0] == 0 && prng_state[1] == 0 && 
       prng_state[2] == 0 && prng_state[3] == 0) {
        prng_state[0] = 1; 
    }
}

uint32_t prng_next(void) {
    const uint32_t result = rotl(prng_state[1] * 5, 7) * 9;
    const uint32_t t = prng_state[1] << 9;

    prng_state[2] ^= prng_state[0];
    prng_state[3] ^= prng_state[1];
    prng_state[1] ^= prng_state[2];
    prng_state[0] ^= prng_state[3];
    prng_state[2] ^= t;
    prng_state[3] = rotl(prng_state[3], 11);

    return result;
}

// Bezpieczne limitowanie zakresu (Rejection Sampling)
uint32_t random_bounded(uint32_t range) {
    uint32_t x;
    uint32_t limit = 0xFFFFFFFF - (0xFFFFFFFF % range);
    do {
        x = prng_next();
    } while (x >= limit);
    return x % range;
}


const char UPPERCASE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char LOWERCASE[] = "abcdefghijklmnopqrstuvwxyz";
const char DIGITS[]    = "0123456789";
const char SPECIALS[]  = "!@#$%^&*()_+-=[]{}|;:,.<>?";

void generate_password(char* output_buffer, int length) {
    char charset[100];
    int charset_size = 0;
    
    for(int i=0; UPPERCASE[i] != '\0'; i++) charset[charset_size++] = UPPERCASE[i];
    for(int i=0; LOWERCASE[i] != '\0'; i++) charset[charset_size++] = LOWERCASE[i];
    for(int i=0; DIGITS[i] != '\0'; i++)    charset[charset_size++] = DIGITS[i];
    for(int i=0; SPECIALS[i] != '\0'; i++)  charset[charset_size++] = SPECIALS[i];

    for(int i = 0; i < length; i++) {
        uint32_t random_index = random_bounded(charset_size);
        output_buffer[i] = charset[random_index];
    }
    
    output_buffer[length] = '\0';
}

int main() {
    // Inicjalizacja systemowego rand() tylko dla mocka ADC
    srand((unsigned int)time(NULL)); 
    
    printf("Inicjalizacja PRNG szumem z symulowanego ADC...\n");
    init_prng();
    
    // Stan początkowy (dobrze to wysłać też przez UART na płytce do debugowania)
    printf("Seed załadowany: %08X %08X %08X %08X\n\n", 
           prng_state[0], prng_state[1], prng_state[2], prng_state[3]);

    char haslo[32]; // Bufor na maksymalnie 31 znaków + \0
    
    printf("Generowanie 5 przykladowych hasel (16 znakow):\n");
    for(int i = 1; i <= 5; i++) {
        generate_password(haslo, 16);
        printf("[%d] %s\n", i, haslo);
    }
    
    return 0;
}