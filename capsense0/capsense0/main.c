//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules

BYTE out_p1;

void main(void)
{
	M8C_EnableGInt ; // Uncomment this line to enable Global Interrupts
	// Insert your main routine code here.
	M8C_EnableIntMask(INT_MSK0, INT_MSK0_GPIO);
	CSD_1_Start();
	CSD_1_InitializeBaselines() ; //scan all sensors first time, init baseline
	CSD_1_SetDefaultFingerThresholds() ; 

	out_p1 = 0x40;
	while(1) {
		BYTE val_p1 = out_p1;

		CSD_1_ScanAllSensors();
		CSD_1_UpdateAllBaselines();

		if(CSD_1_bIsSensorActive(0)) {
			val_p1 |= 0x04;  // BLUE LED HI (touch sw indicator)
			val_p1 &= ~0x40; // SENSEOUT LO
		} else {
			val_p1 &= ~0x04; // BLUE LED LO
			val_p1 |= 0x40;  // SENSEOUT HI (Hi-Z)
		}
		out_p1 = val_p1;
	}
}


#pragma interrupt_handler GPIO_INT
void GPIO_INT(void)
{
	// P0_2: keyboard matrix drive signal (active-lo)
	PRT1DR = (PRT0DR & 0x04)? 0x40 : out_p1;
}

