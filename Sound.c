/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	Sound.c: DAC Init, Tone Tables, and functions for the song logic
*/

#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "SysTick.h"
#include "Buttons.h"
#include "PianoKeys.h"
#include <stdint.h>
#include <stdbool.h>

//Macro Constants - preprocessor directive
#define DAC (*((volatile unsigned long *)0x400050FC)) // PB0-PB5 
#define Speaker_Toggle_Mask     0x00000008  // mask used to toggle the speaker output
#define NUM_VALs  128 // Assume 3-bit DAC is used, that will give 16 values for one period.
#define NUM_NOTEs 8   // number of notes to be played repeatedly
#define PAUSE 255				// assume there are less than 255 tones used in any song
#define MAX_NOTES 50  // assume maximum number of notes in any song is 100. You can change this value if you add a long song.
#define NUM_SONGS 3   // number of songs in the play list.
#define SILENCE MAX_NOTES // use the last valid index to indicate a silence note. The song can only have up to 254 notes. 
#define NUM_OCT  3   // number of octave defined in tontab[]
#define NUM_NOTES_PER_OCT 7  // number of notes defined for each octave in tonetab
#define NVIC_EN0_PORTF 0x40000000  // enable PORTF edge interrupt
#define NVIC_EN0_PORTD 0x00000008  // enable PORTD edge interrupt
#define NUM_SAMPLES 128  // number of sample in one sin wave period

//Function Prototypes
void Delay(void);

//Global variables
unsigned char i,j,input,previous; 
unsigned char tone_index = 0;	
volatile uint8_t curr_song=0;      // 0: Happy Birthday, 1: Mary Had A Little Lamb. 2: Twinkle Twinkle Little Stars
volatile uint8_t stop_play=1;      // 0: continue playing a song, 1: stop playing a song
volatile uint8_t octave=0;         // 0: lower C, 1: middle C, 2: upper C
bool first_note = false;
volatile uint8_t curr_mode=PIANO;  // 0: piano mode, 1: auto-play mode
int currentSong = 0;
bool musicOn = false; 

// 6-bit 64 element sine wave
const unsigned char SineWave[134] = {34, 35, 37, 38, 40, 41, 42, 44, 45, 47, 48, 49, 50, 52, 53, 54, 55, 56, 57, 58, 59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 63, 62, 62, 62, 62, 61, 61, 60, 60, 59, 59, 58, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 45, 44, 42, 41, 40, 39, 38, 37, 35, 34, 32, 30, 29, 27, 26, 24, 23, 22, 20, 19, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 19, 20, 22, 23, 24, 26, 27, 29, 30, 32, 34, 35, 37, 38};

// initial values for piano major notes: assume SysTick clock is 16MHz.
const unsigned long Tone_Tab[] =
// initial values for three major notes for 16MHz system clock
// Note name: C, D, E, F, G, A, B
// Offset:0, 1, 2, 3, 4, 5, 6
{30534*4, 27211*4, 24242*4, 22923*4, 20408*4, 18182*4, 16194*4, // C3 major notes
 30534*2,27211*2,24242*2,22923*2,20408*2,18182*2,16194*2, // C4 major notes
 15289*2,13621*2,12135*2,11454*2,10204*2,9091*2,8099*2, // C5 major notes
 7645*2,6810*2,6067*2,5727*2,5102*2,4545*2,4050*2};// C6 major notes

// Index for notes used in music scores
enum Notes{
		C3,
		D3,
		E3,
		F3,
		G3,
		A4,
		B4,
		C4,
    D4,
    E4,
    F4,
    G4,
		A5,
		B5,
    C5,
    D5,          
    E5,          
    F5,          
    G5,          
    A6,         
    B6,          
    C6,
		D6,              
    E6,             
    F6,              
    G6,
		A7,
		B7
};

// doe ray mi fa so la ti 
// C   D   E  F  G  A  B
NTyp Score_Tab[][MAX_NOTES] = {  

// Mary Had A Little Lamb
 {
  E3,4, D3,4, C3,4, D3,4, E3,4, E3,4, E3,8, PAUSE,4,  
  D3,4, D3,4, D3,8, PAUSE,4, E3,4, G3,4, G3,8, PAUSE,4,
  E3,4, D3,4, C3,4, D3,4, E3,4, E3,4, E3,4, E3,4, 
  D3,4, D3,4, E3,4, D3,4, C3,8, PAUSE,4, 0,0
	},
 
// score table for Twinkle Twinkle Little Stars
{C3,4,C3,4,G3,4,G3,4,A4,4,A4,4,G3,8,F3,4,F3,4,E3,4,E3,4,D3,4,D3,4,C3,8, 
 G3,4,G3,4,F3,4,F3,4,E3,4,E3,4,D3,8,G3,4,G3,4,F3,4,F3,4,E3,4,E3,4,D3,8, 
 C3,4,C3,4,G3,4,G3,4,A4,4,A4,4,G3,8,F3,4,F3,4,E3,4,E3,4,D3,4,D3,4,C3,8,0,0},
	
// score table for Happy Birthday
{//so   so   la   so   doe' ti
   G3,2,G3,2,A4,4,G3,4,C4,4,B4,4,
// pause so   so   la   so   ray' doe'
   PAUSE,4,  G3,2,G3,2,A4,4,G3,4,D4,4,C4,4,
// pause so   so   so'  mi'  doe' ti   la
   PAUSE,4,  G3,2,G3,2,G4,4,E4,4,C4,4,B4,4,A4,4, 
// pause fa'  fa'   mi'  doe' ray' doe' stop
	 PAUSE,4,  F3,2,F3,2, E3,4,C3,4,D3,4,C3,8,0,0},

// Anthony's Song
{F4, 2, E4, 2, D4, 2, C4, 2, D4, 2, A4, 2, B4, 4, 
 G3, 4, D3, 2, E3, 2, F3, 4, G3, 4, C3, 4,
 A4, 8, F3, 2, E3, 2, D3, 2, C3, 2, D3, 2, A4, 2, B4, 4, 
 G3, 4, D3, 2, E3, 2, F3, 4, E3, 4, C3, 4, D3, 8 , 0, 0 }
};

uint8_t get_current_mode(void)
{
	return curr_mode;
}

void change_current_mode(void)
{
	if (get_current_mode()==0){
		curr_mode = AUTO_PLAY;
	}
	else{
		curr_mode = PIANO;
	}
}




void play_a_song(void)
{
	int i = 0, j;
	first_note = false; 
	// Iterate through the notes until we hit an end marker (0,0)
	while (Score_Tab[currentSong][i].delay && !first_note && musicOn) {  // Use [0][i] to iterate within the first song row
		if (Score_Tab[currentSong][i].tone_index == PAUSE) // Handles blank note boundery case
			SysTick_stop();  // Silence tone
		else {
			SysTick_Set_Current_Note((Tone_Tab[Score_Tab[currentSong][i].tone_index])/(128*(octave+1))); 
			//Set reload with the current note's reload divided by the number of samples (divided again by an octave counter 
			//	which accounts for a decreased reload (higher frequency) to maintin the pitch
			SysTick_start();
		}
		
		// Tempo control: play note for the duration specified in the score table
		for (j = 0; j < Score_Tab[currentSong][i].delay; j++) 
			Delay();
			Delay();

		
		SysTick_stop();
		i++;  // Move to the next note
	}

	// Pause after each play
	for (j = 0; j < 7; j++) 
		Delay();
}


void next_song(void)
{
	first_note= true; //causes 
	currentSong = (currentSong + 1) % 4; //mod 4 to round robin through all 4 songs
}

void reset_song(void)
{
	currentSong = 0; 
}


unsigned char is_music_on(void)
{
  if(musicOn){
		return 1; 
	}
	else{
		return 0; 
	}
		
}

void turn_off_music(void)
{
	musicOn = false; 
}

void turn_on_music(void)
{
	musicOn = true;
}

// Subroutine to wait 0.1 sec
// Inputs: None
// Outputs: None
// Notes: ...
void Delay(void){
	unsigned long volatile time;
  time = 727240*17/91;  // 0.1sec for 16MHz
//  time = 727240*100/91;  // 0.1sec for 80MHz
  while(time){
		time--;
  }
}

																		// **************DAC_Init*********************
// Initialize 6-bit DAC 
// Input: none  
// Output: none
void DAC_Init(void){unsigned long volatile delay;
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
	delay = SYSCTL_RCGC2_R;    // allow time to finish activating
	GPIO_PORTB_AMSEL_R &= ~0x3F;      // no analog 
	GPIO_PORTB_PCTL_R &= ~0x000FFFFF; // regular function
	GPIO_PORTB_DIR_R |= 0x3F;      // make PB5-0 out
	GPIO_PORTB_AFSEL_R &= ~0x3F;   // disable alt funct on PB5-0
	GPIO_PORTB_DEN_R |= 0x3F;      // enable digital I/O on PB5-0
	GPIO_PORTB_DR8R_R |= 0x3F;        // enable 8 mA drive on PB5-0
  }