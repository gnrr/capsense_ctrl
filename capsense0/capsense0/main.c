//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules

BYTE out_ctrl, out_shift, out_led;
BYTE p0_old;

void main(void)
{
	M8C_EnableGInt ; // Uncomment this line to enable Global Interrupts
	// Insert your main routine code here.
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);
	CSD_1_Start();
	CSD_1_InitializeBaselines() ; //scan all sensors first time, init baseline
	CSD_1_SetDefaultFingerThresholds() ; 

//PRT1DR = 0x00;
//PRT1DR = 0x80;
//	PRT1DR = 0xC0;
	while(1) {
		BYTE ctrl, shift, led;
		CSD_1_ScanAllSensors();
		CSD_1_UpdateAllBaselines();

		// CTRL:  P0_2 -> P1_6
		if(CSD_1_bIsSensorActive(0)) {
			ctrl = 0x80;	// 10xx_xxxx: CTRL SENSEOUT LO
			led |= 0x04;	// xxxx_0100: BLUE LED ON (touch sw indicator)
		} else {
			ctrl = 0xC0;    // 11xx_xxxx: CTRL SENSEOUT HI (Hi-Z)
			led &= 0x0B;	//  ^
		}

		// SHIFT: P0_3 -> P1_7
		if(CSD_1_bIsSensorActive(1)) {
			shift = 0x40;	// 01xx_xxxx: SHIFT SENSEOUT LO
			led  |= 0x08;	// xxxx_1000: RED LED ON (touch sw indicator)
		} else {
			shift = 0xC0;   // 11xx_xxxx: SHIFT SENSEOUT HI (Hi-Z)
			led  &= 0x07;	// ^
		}

		out_ctrl = ctrl;
		out_shift = shift;
		PRT1DR = (PRT1DR & 0xC0) | led;
	}
}


#pragma interrupt_handler GPIO_INT
void GPIO_INT(void)
{
	// output keyboard matrix drive signal (active-lo)
	BYTE p0, diff;
	
	p0 = PRT0DR & 0x0C;
	diff = (p0 ^ p0_old);
	if(diff == 0x00) return;
	
	if(diff & 0x04)
		PRT1DR = (p0 & 0x04)? 0xC0 : out_ctrl;
	else if(diff & 0x08)
		PRT1DR = (p0 & 0x08)? 0xC0 : out_shift;

	p0_old = p0;
}

