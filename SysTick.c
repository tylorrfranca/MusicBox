/*
Project1Part1
systick.c
Runs on LM4F120/TM4C123
Starter File for CECS 447 Project 1 Digital Piano
Project Group Number: 1
Team Members : Ivan Martinez, Anthony Keroles, Diego Davalos, Tylor Franca Pires
Date: 02/04/2025
File Description : This file initilizes systick that is used for our music
*/

#include "tm4c123gh6pm.h"
#include "SysTick.h"


#define SPEAKER (*((volatile unsigned long *)0x40004020)) // define SPEAKER connects to PA3: 100
#define Speaker_Toggle_Mask     0x00000008  // mask used to toggle the speaker output

unsigned char Index;  


//#define SPEAKER (*((volatile unsigned long *)______________))

// Initialize SysTick with interrupt priority 2. Do not start it.
void SysTick_Init(void){
	Index = 0;
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_CLK_SRC+NVIC_ST_CTRL_INTEN; // "NVIC_ST_CTRL_CLK_SRC" - systick uses main system clock
																														 // "NVIC_ST_CTRL_INTEN" - enables SysTick interrupt, allowing to trigger ISR when it rolls over
}

void SysTick_start(void)
{
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}
void SysTick_stop(void)
{
	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE;
}

void SysTick_Set_Current_Note(unsigned long n_value) //Function uses note (frequency) length as a reload value to set systick's square wave length to output to the speaker
{
  NVIC_ST_RELOAD_R = n_value-1;// update reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it
	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;
}

// Interrupt service routine,
// frequency is determined by current tone being played
void SysTick_Handler(void){
	//	NVIC_ST_CTRL_R &= ~NVIC_ST_CTRL_ENABLE; // Disable SysTick to prevent double interrupts
		Index = (Index + 1) & 0x7F;  // Wrap around for 128 samples per wave period
	//  NVIC_ST_CURRENT_R = 0;  // Reset SysTick counter
	  GPIO_PORTF_DATA_R ^= 0x08;  // Toggle PF3 for debugging
	//	NVIC_ST_CTRL_R |= NVIC_ST_CTRL_ENABLE;  // Re-enable SysTick
		DAC = SineWave[Index];  // Output sine wave value to DAC (PB5-0)
	}
