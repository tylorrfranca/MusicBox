/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	Buttons.c: Initializes Port F for buttons (PF0,PF4) as inputs with interrupt priority 5
		PF0 - Switch 2, Autoplay mode = round robin song selection, PianoKeys mode = round robin octave selection
		PF4 - Switch 1, Autoplay mode = on/off, PianoKeys mode = 
*/

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "Buttons.h"
#include "Sound.h"

// Constants
#define SW1 0x10  // bit position for onboard switch 1(left switch) PF4
#define SW2 0x01  // bit position for onboard switch 2(right switch) PF0
#define NVIC_EN0_PORTF 0x40000000  // enable PORTF edge interrupt

// Globals
uint8_t Btns = SW1|SW2; // Buttons = 0x11;
volatile int debounceDelay = 1000; // Adjust this value as needed for ~10-20ms delay (depends on clock speed)

//onboard buttons
void Buttons_Init(void){ 
	unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; // (a) activate clock for port F
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY; // unlock GPIO Port F
  GPIO_PORTF_CR_R |= Btns;         // allow changes to PF4,0
  GPIO_PORTF_DIR_R &= ~Btns;    // (c) make PF4 and PF0 inputs (built-in button)
  GPIO_PORTF_AFSEL_R &= ~Btns;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= Btns;     //     enable digital I/O on PF4,0
  GPIO_PORTF_PCTL_R &= ~0x000F000F; //  configure PF4,0 as GPIO
  GPIO_PORTF_AMSEL_R &= ~Btns;  //     disable analog functionality on PF4,0
  GPIO_PORTF_PUR_R |= Btns;     //     enable weak pull-up on PF4,0
  GPIO_PORTF_IS_R &= ~Btns;     // (d) PF4,PF0 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~Btns;    //     PF4,PF0 is not both edges
  GPIO_PORTF_IEV_R |= Btns;    //     PF4,SW1,SW2 rising edge event
  GPIO_PORTF_ICR_R = Btns;      // (e) clear flags 4,0
  GPIO_PORTF_IM_R |= Btns;      // (f) arm interrupt on PF4,PF0
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF1FFFFF)|0x00A00000; // (g) bits:23-21 for PORTF, set priority to 5
  NVIC_EN0_R |= (1 << 30);           // 9) enable IRQ 30 in NVIC	
}

void GPIOPortF_Handler(void) {
    if (GPIO_PORTF_RIS_R & 0x10) {  // SW1 Pressed
				for (uint32_t time=0;time<72724*2;time++) {}
        GPIO_PORTF_ICR_R = 0x10;  // Acknowledge flag
				change_current_mode();
				if (get_current_mode()){   // 0 is piano mode    1 is auto_play mode
					if (is_music_on()) {
							turn_off_music();
					} else {
							reset_song(); // Reset to first song
							turn_on_music();
					}
				}else{
					turn_off_music();
				}
    }

    if (GPIO_PORTF_RIS_R & 0x01) {  // SW2 Pressed
			for (uint32_t time=0;time<72724*2;time++) {}
      GPIO_PORTF_ICR_R = 0x01;  // Acknowledge flag
			if (get_current_mode()){   // 0 is piano mode    1 is auto_play mode
				if(is_music_on()){
					turn_off_music();
					next_song();
					turn_on_music();
				}
			}
			else{
				octave = (octave + 1) % 3;
			}
    }
}


