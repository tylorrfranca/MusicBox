/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	PianoKeys.c: Initializes Port D for buttons (PD0-PD3) as positive logic inputs (pull down resistors) with priority 3
		PD0 - C doe
		PD1 - D ray
		PD2 - E mi
		PD3 - F fa
*/

//headers
#include "tm4c123gh6pm.h" //enclosed header in quotations: compiler first checks the directory of source file 
#include <stdint.h> //enclosed header in angle brackets: compiler only checks the system directories
#include "PianoKeys.h"
#include "Sound.h"
#include "SysTick.h"

//constants
#define C_Note 0x01  // bit position for PD0 Button
#define D_Note 0x02  // bit position for PD1 Button
#define E_Note 0x04  // bit position for PD2 Button
#define F_Note 0x08  // bit position for PD3 Button
#define NVIC_EN0_PORTD 0x00000008  // enable PORTD edge interrupt

//globals
uint8_t Notes = C_Note | D_Note | E_Note | F_Note; // Notes = 0x0F

//no need to unlock. Only PD7 needs to be unlocked. 
void PianoKeys_Init(void){
	unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; // (a) activate clock for port D
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTD_LOCK_R = GPIO_LOCK_KEY; // unlock GPIO Port D
  GPIO_PORTD_CR_R |= Notes;         // allow changes to PD0-3
  GPIO_PORTD_DIR_R &= ~Notes;    // (c) make PD0-3 inputs (built-in button)
  GPIO_PORTD_AFSEL_R &= ~Notes;  //     disable alt funct on PFD0-3
  GPIO_PORTD_DEN_R |= Notes;     //     enable digital I/O on PFD0-3
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; //  configure PD0-3 as GPIO
  GPIO_PORTD_AMSEL_R &= ~Notes;  //     disable analog functionality on PD0-3
  GPIO_PORTD_PDR_R |= Notes;     //     enable weak pull-up on PD0-3
  GPIO_PORTD_IS_R &= ~Notes;     // (d) PD0-3 is edge-sensitive
  GPIO_PORTD_IBE_R &= ~Notes;    //     PD0-3 is both edges
  GPIO_PORTD_IEV_R |= Notes;    //     PD0-3 rising edge event
  GPIO_PORTD_ICR_R = Notes;      // (e) clear flags 0-3
  GPIO_PORTD_IM_R |= Notes;      // (f) arm interrupt on PD0-3
  NVIC_PRI0_R = (NVIC_PRI0_R&0x1FFFFFFF)|0x60000000; // (g) bits:31-29 for PORTD, set priority to 3
  NVIC_EN0_R |= (1 << 3);           // 9) enable IRQ 3 in NVIC
}

// Dependency: Requires PianoKeys_Init to be called first, assume at any time only one key is pressed
// Inputs: None
// Outputs: None
// Description: Rising/Falling edge interrupt on PD6-PD0. Whenever any 
// button is pressed, or released the interrupt will trigger.
void GPIOPortD_Handler(void) {
	if ((get_current_mode() == 0)){   // 0 is piano mode    1 is auto_play mode
  while (GPIO_PORTD_DATA_R & 0x01) {  // Stay in loop while PD0 is pressed
	  for (uint32_t time = 0; time < 72724; time++) {}  // Small delay
	  SysTick_Set_Current_Note(Tone_Tab[0 + (7*octave)] / NUM_VALs); // Play note
	  SysTick_start();
	  Delay();  // Delay for tempo
		  }
	  GPIO_PORTD_ICR_R = 0x01;  // Acknowledge flag *after* button is released
	  
  while (GPIO_PORTD_DATA_R & 0x02) {  // Stay in loop while PD0 is pressed
	  for (uint32_t time = 0; time < 72724; time++) {}  // Small delay
	  SysTick_Set_Current_Note(Tone_Tab[1 + (7*octave)] / NUM_VALs); // Play note
	  SysTick_start();
	  Delay();  // Delay for tempo
		  }
  GPIO_PORTD_ICR_R = 0x02;  // Acknowledge flag *after* button is released
	  
  while (GPIO_PORTD_DATA_R & 0x04) {  // Stay in loop while PD0 is pressed
	  for (uint32_t time = 0; time < 72724; time++) {}  // Small delay
	  SysTick_Set_Current_Note(Tone_Tab[2 + (7*octave)] / NUM_VALs); // Play note
	  SysTick_start();
	  Delay();  // Delay for tempo
		  }
  GPIO_PORTD_ICR_R = 0x04;  // Acknowledge flag *after* button is released
	  
  while (GPIO_PORTD_DATA_R & 0x08) {  // Stay in loop while PD0 is pressed
	  for (uint32_t time = 0; time < 72724; time++) {}  // Small delay
	  SysTick_Set_Current_Note(Tone_Tab[3 + (7*octave)] / NUM_VALs); // Play note
	  SysTick_start();
	  Delay();  // Delay for tempo
		  }
		  SysTick_stop();	
		  GPIO_PORTD_ICR_R = 0x08;  // Acknowledge flag *after* button is released
	  }
		  GPIO_PORTD_ICR_R = 0x01;  // Acknowledge flag *after* button is released
		  GPIO_PORTD_ICR_R = 0x02;  // Acknowledge flag *after* button is released
		  GPIO_PORTD_ICR_R = 0x04;  // Acknowledge flag *after* button is released
		  GPIO_PORTD_ICR_R = 0x08;  // Acknowledge flag *after* button is released
}

