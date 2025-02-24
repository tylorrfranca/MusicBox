/*
	Project Group Number: 1
	Diego Davalos, Anthony Keroles, Tylor Franca Pires, Ivan Martinez
	February 11, 2025
	systick.h This file header contains the function prototypes for the systick.c file.
*/
#ifndef SYSTICK_H
#define SYSTICK_H
// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void);
void SysTick_start(void);
void SysTick_stop(void);
void SysTick_Set_Current_Note(unsigned long n_value);
#define DAC (*((volatile unsigned long *)0x400050FC)) // PB0-PB5 
extern const unsigned char SineWave[134];
#endif

