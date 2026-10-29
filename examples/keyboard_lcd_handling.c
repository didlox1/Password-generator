#include "MKL05Z4.h"
#include "frdm_bsp.h"
#include "lcd1602.h"
#include "klaw.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


volatile uint8_t S2_press=0;
volatile uint8_t S3_press=0;
volatile uint8_t S4_press=0;

void PORTA_IRQHandler(void)	// Interrupt handling
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

int main (void) 
{
	// Keyboard Init
	Klaw_Init();
	Klaw_S2_4_Int();

	// LCD Init
	LCD1602_Init();
	LCD1602_Backlight(TRUE);
	LCD1602_ClearAll();
	LCD1602_Print("Initialization");
	LCD1602_SetCursor(6, 1);
	LCD1602_Print("of the LCD");
	
  while(1)
	{
		if(S2_press)
		{
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Switch nr. 2");
			LCD1602_SetCursor(7,1);
			LCD1602_Print("pressed");
			S2_press=0;
		}
		if(S3_press)
		{
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Switch nr. 3");
			LCD1602_SetCursor(7,1);
			LCD1602_Print("pressed");
			S3_press=0;
		}
		if(S4_press)
		{
			LCD1602_ClearAll();
			LCD1602_SetCursor(0,0);
			LCD1602_Print("Switch nr. 4");
			LCD1602_SetCursor(7,1);
			LCD1602_Print("pressed");
			S4_press=0;
		}
	}
}
