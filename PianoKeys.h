/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	PianoKeys.h: Initializes Port D for buttons (PD0-PD3) as positive logic inputs (pull down resistors) with priority 3
		PD0 - C
		PD1 - D
		PD2 - E
		PD3 - F
*/

#include <stdint.h>

void PianoKeys_Init(void);