#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "klaw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "password.h"
#include "generator.h"
#include "ADC.h"
#include "uart.h"

volatile uint8_t S2_press=0; // password generation
volatile uint8_t S3_press=0; // seed generation
volatile uint8_t S4_press=0; // UART transmission

volatile uint8_t wynik_ok=0;
volatile uint16_t temp;
volatile uint16_t	wynik;

////////////////////
// Sending variables

#define N_SEEDS      4u
#define N_SAMPLES    100u

volatile uint8_t seed_harvesting = 0;
uint32_t harvested_seeds[N_SEEDS];
uint32_t seed_collect_value = 0;
uint32_t seed_collect_samples = 0;
uint32_t seed_collect_index = 0;

//////////////////////////
// Password generator init
	state_t state = {
    .xoshiro = {
        0x12345678u,
        0x9ABCDEF0u,
        0x13572468u,
        0x24681357u
    },
    .lcg = 0x12345678u
	};

  password_profile_t profile = {
      .length = 16,
      .lowercase = true,
      .uppercase = true,
      .digits = true,
      .specials = SPECIAL_SAFE
  };

  char password[16];
	
	void generate_LCG69069(){
		bool success = password_generate(
			password,
			sizeof(password),
			&profile,
			LCG69069,
			&state
		);
			
		if (!success) {
			strcpy(password, "Failed");
		}
	}
	
	void generate_Xoshiro128(){
		bool success = password_generate(
			password,
			sizeof(password),
			&profile,
			Xoshiro128,
			&state
		);
			
		if (!success) {
			strcpy(password, "Failed");
		}
	}
	
////////////////////
// Sending helpers
	
static void start_adc_conversion(void)
{
    ADC0->SC1[0] =
        ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(8);
}

static void uart_print_number(uint32_t number)
{
    char text[16];
    sprintf(text, "%lu\r\n", (unsigned long)number);
    uart_print(text);
}

void generate_seed(void)
{
    seed_collect_value = 0;
    seed_collect_samples = 0;
    seed_collect_index = 0;
    seed_harvesting = 1;

    start_adc_conversion();
}

void send_seeds(void)
{
    for (uint32_t i = 0; i < N_SEEDS; ++i) {
        uart_print_number(harvested_seeds[i]);
    }
}

void generate_Xoshiro_samples(void)
{
    for (uint32_t i = 0; i < N_SAMPLES; ++i) {
        uart_print_number(Xoshiro128(&state));
    }
}

void generate_LCG_samples(void)
{
    for (uint32_t i = 0; i < N_SAMPLES; ++i) {
        uart_print_number(LCG69069(&state));
    }
}

	
/////////////////////
// Interrupt handlers

void PORTA_IRQHandler(void)
{
	uint32_t buf;
	buf=PORTA->ISFR & (S2_MASK | S3_MASK | S4_MASK);

	switch(buf)
	{
		case S2_MASK:	DELAY(100)
									if(!(PTA->PDIR&S2_MASK))		// Debouncing cancelation
									{
										DELAY(100)
										if(!(PTA->PDIR&S2_MASK))
										{
											if(!S2_press)
											{
												S2_press=1;
											}
										}
									}
									break;
		case S3_MASK:	DELAY(100)
									if(!(PTA->PDIR&S3_MASK))		// Debouncing cancelation
									{
										DELAY(100)
										if(!(PTA->PDIR&S3_MASK))
										{
											if(!S3_press)
											{
												S3_press=1;
											}
										}
									}
									break;
		case S4_MASK:	DELAY(100)
									if(!(PTA->PDIR&S4_MASK))		// Debouncing cancelation
									{
										DELAY(100)
										if(!(PTA->PDIR&S4_MASK))
										{
											if(!S4_press)
											{
												S4_press=1;
											}
										}
									}
									break;
		default:			break;
	}	
	PORTA->ISFR |=  S2_MASK | S3_MASK | S4_MASK;	// Clearing ISF bits
	NVIC_ClearPendingIRQ(PORTA_IRQn);
}

void ADC0_IRQHandler()
{	
	temp = ADC0->R[0];	// Odczyt danej i skasowanie flagi COCO
	if(!wynik_ok)				// SprawdŸ, czy wynik skonsumowany przez pêtlê g³ówn¹
	{
		wynik = temp;			// Wyœlij now¹ dan¹ do pêtli g³ównej
		wynik_ok=1;
	}
}


// ------------
// Main program

int main (void) 
{
	// Keyboard Init
	Klaw_Init();
	Klaw_S2_4_Int();

	// LCD Init
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	LCD1602_Init();
	LCD1602_Backlight(TRUE);
	LCD1602_ClearAll();
	LCD1602_Print("Initialization");
	LCD1602_SetCursor(6, 1);
	LCD1602_Print("of the LCD");
	
	// ADC Init
	uint8_t	kal_error;
	kal_error=ADC_Init();				// Inicjalizacja i kalibracja przetwornika A/C
	if(kal_error)
	{	
		while(1);									// Klaibracja siê nie powiod³a
	}
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(8);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 8 i odblokowanie przerwania
	
	// UART Init
	uart_status_t uart_error;
	uart_error = uart_init(115200, UART_PTB2_RX, UART_PTB1_TX);
	// Main loop
  while(1)
	{
		if (S2_press)
		{
			generate_seed();
			S2_press = 0;
		}
		
		if (S3_press)
		{
			generate_Xoshiro_samples();
			S3_press = 0;
		}
		
		if (S4_press)
		{
			generate_LCG_samples();
			S4_press = 0;
		}
		
		if (wynik_ok)
		{
			if (seed_harvesting)
			{
				seed_collect_value =
						(seed_collect_value << 1) |
						(uint32_t)(wynik & 1u);
		
				seed_collect_samples++;
		
				if (seed_collect_samples == 32u)
				{
					harvested_seeds[seed_collect_index] =
						seed_collect_value;
		
					seed_collect_index++;
					seed_collect_value = 0;
					seed_collect_samples = 0;
		
					if (seed_collect_index == N_SEEDS)
					{
						seed_harvesting = 0;
						seed_collect_index = 0;
		
						xoshiro_seed(&state, harvested_seeds);
						send_seeds();
					}
				}
			}
		
			wynik_ok = 0;
		
			if (seed_harvesting)
			{
					start_adc_conversion();
			}
		}
	} // Main loop
}
