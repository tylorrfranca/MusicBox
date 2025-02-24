/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	Buttons.h: Initializes Port F for buttons (PF0,PF4) as inputs with interrupt priority 5
		PF0 - Switch 2, Autoplay mode = round robin song selection, PianoKeys mode = round robin octave selection
		PF4 - Switch 1, Autoplay mode = on/off, PianoKeys mode = 
*/

#include <stdint.h>

void Buttons_Init(void);




