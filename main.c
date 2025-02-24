/*
	Project1Part1
	main.c
	Project Group Number: 1
	Diego Davalos, Anthony Keroles, Tylor Franca Pires, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	Runs on TM4C
	Port B bits 0-5 outputs to the 6-bit DAC
		B0 - MSB
		B6 - LSB
	Port F is onboard LaunchPad switches and LED
	Port D bits 0-3 inputs from piano keys: CDEF:doe ray mi fa,negative logic connections.
		D0 - C
		D1 - D
		D2 - E
		D3 - F
	SysTick ISR: PF3 is used to implement heartbeat
*/

// Header files 
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Buttons.h"
#include "PianoKeys.h"
#include "SysTick.h"

// Function Prototypes
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);
  
// Subroutines Section - MAIN: Mandatory for a C Program to be executable
int main(void){
  DisableInterrupts();    
  DAC_Init();        // Port B 
	PianoKeys_Init();  // Port D
	Buttons_Init();  // Port F
	SysTick_Init();
	EnableInterrupts();
	
  while(1){
		switch (get_current_mode()) {
			case PIANO:
				WaitForInterrupt();
				break;
			case AUTO_PLAY:
			play_a_song();
				break;
			default:
				break;
  	}
  }
}




