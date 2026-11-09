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

// -----------------------
// Password generator init
	simple_rng_t rng = {
        .state = 123456789u
  };

  password_profile_t profile = {
      .length = 16,
      .lowercase = true,
      .uppercase = true,
      .digits = true,
      .specials = SPECIAL_SAFE
  };

  char password[16];
	
	void generate(){
	bool success = password_generate(
		password,
		sizeof(password),
		&profile,
		simple_random,
		&rng
	);
		
	if (!success) {
		strcpy(password, "Failed");
	}
}
	
// -----------------
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
	if(!wynik_ok)				// Sprawd�, czy wynik skonsumowany przez p�tl� g��wn�
	{
		wynik = temp;			// Wy�lij now� dan� do p�tli g��wnej
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
		while(1);									// Klaibracja si� nie powiod�a
	}
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(8);		// Pierwsze wyzwolenie przetwornika ADC0 w kanale 8 i odblokowanie przerwania
	
	// UART Init
	uart_status_t uart_error;
	uart_error = uart_init(115200, UART_PTB2_RX, UART_PTB1_TX);
	// Main loop
  while(1)
	{
		if(S2_press)
		{
			generate();
			// LCD
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Generated pass:");
			LCD1602_SetCursor(0,1);
			LCD1602_Print(password);
			S2_press=0;
		}
		if(S3_press)
		{
			ADC0->SC1[0] |= ADC_SC1_ADCH(8); // Wyzwolenie programowe przetwornika ADC0 w kanale 8
			S3_press = 0;
		}
		if(wynik_ok)
		{
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Generated seed:");
			sprintf(display,"Seed = 0x%x",wynik);
			LCD1602_SetCursor(0,1);
			LCD1602_Print(display);	
			wynik_ok=0;
		}
		
		if(S4_press)
		{
			if (uart_error != UART_OK){
				LCD1602_SetCursor(0,0);
				LCD1602_ClearAll();
				LCD1602_Print("UART CONFIGERR");
			}
			else{
				uart_print("Button nr.4 pressed\n");
				LCD1602_SetCursor(0,0);
				LCD1602_ClearAll();
				LCD1602_Print("UART printed");
			}
			S4_press=0;
		}
	}
}
