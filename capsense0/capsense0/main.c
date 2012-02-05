//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules

BYTE out_ctrl, out_shift;
BYTE p0_old = 0xFF;

void main(void)
{
	M8C_EnableGInt ; // Uncomment this line to enable Global Interrupts
	// Insert your main routine code here.
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);
	CSD_1_Start();
	CSD_1_InitializeBaselines() ; //scan all sensors first time, init baseline
	CSD_1_SetDefaultFingerThresholds() ; 

	while(1) {
		CSD_1_ScanAllSensors();
		CSD_1_UpdateAllBaselines();

		// CTRL:  P0_2 -> P1_6
		if(CSD_1_bIsSensorActive(0)) {
			out_ctrl = 0xB4;	// 1011_xxxx: CTRL SENSEOUT LO
								// xxxx_0100: BLUE LED ON (touch sw indicator)
		} else {
			out_ctrl = 0xF0;    // 1111_xxxx: CTRL SENSEOUT HI (Hi-Z)
								//  ^
		}

		// SHIFT: P0_3 -> P1_7
		if(CSD_1_bIsSensorActive(1)) {
			out_shift = 0x78;	// 0111_xxxx: SHIFT SENSEOUT LO
								// xxxx_1000: RED LED ON (touch sw indicator)
		} else {
			out_shift = 0xF0;   // 1111_xxxx: SHIFT SENSEOUT HI (Hi-Z)
								// ^
		}
	}
}


#pragma interrupt_handler GPIO_INT
void GPIO_INT(void)
{
	// output keyboard matrix drive signal (active-lo)
	BYTE p0, diff;
	
	p0 = PRT0DR;
	diff = p0 ^ 0x0C;
	
	if(diff == 0x00) return;
	
	if(diff & 0x04)
		PRT1DR = (p0 & 0x04)? 0xF0 : out_ctrl;
	else if(diff & 0x08)
		PRT1DR = (p0 & 0x08)? 0xF0 : out_shift;

	p0_old = p0;
}

