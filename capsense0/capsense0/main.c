//----------------------------------------------------------------------------
// C main line
//----------------------------------------------------------------------------

#include <m8c.h>        // part specific constants and macros
#include "PSoCAPI.h"    // PSoC API definitions for all User Modules


void main(void)
{
	// M8C_EnableGInt ; // Uncomment this line to enable Global Interrupts
	// Insert your main routine code here.
	M8C_EnableGInt;
	CSD_1_Start(); 
	CSD_1_InitializeBaselines() ; //scan all sensors first time, init baseline
	CSD_1_SetDefaultFingerThresholds() ; 
	PWM8_1_Start();
	while(1) {
		CSD_1_ScanAllSensors();
		CSD_1_UpdateAllBaselines();
		if(CSD_1_bIsSensorActive(0))
			PWM8_1_WritePulseWidth(255);
		else
			PWM8_1_WritePulseWidth(10);
	}
	
}
