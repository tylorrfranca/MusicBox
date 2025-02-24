/*
	Diego Davalos, Antony Keroles, Tylor Franca, Ivan Martinez
	February 11, 2025
	Version 1.0.0
	Sound.h: DAC Init, Tone Tables, and functions for the song logic
*/
#include <stdint.h>
//Header Guards to prevent double declaration and allows for use externally (if sound.h hasnt yet been defined, then define it to declare these elements)
//	Static variables should be declared with extern since static variables are limited to scope its defined in (internal linkage)
#ifndef SOUND_H
#define SOUND_H

void change_current_mode(void);
uint8_t get_current_mode(void);
extern volatile uint8_t octave; //extern = defined elsewhere
void SysTick_Set_Current_Note(unsigned long n_value);
void Delay(void);
#define NUM_VALs  128 //macros defined too
extern const unsigned long Tone_Tab[];

#endif //SOUND_H

// define music note data structure 
struct Note {
  uint32_t tone_index;
  uint8_t delay;
};
typedef const struct Note NTyp;

// Constants
#define NUM_SONGS 3

#define PIANO     0  // piano mode: press a key to play a tone
#define AUTO_PLAY 1  // auto play mode: automatic playing a song

//Function Prototypes
void DAC_Init(void);
// **************Sound_Start*********************
// Set reload value and enable systick timer
// Input: time duration to be generated in number of machine cycles
// Output: none
void Sound_Start(unsigned long period);
void Sound_stop(void);
void play_a_song();
void Music_Init(void);
void next_song(void);
unsigned char is_music_on(void);
void reset_song(void);
void turn_off_music(void);
void turn_on_music(void);
void SysTick_Init(void);
void SysTick_start(void);
void SysTick_stop(void);





