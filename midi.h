// conf.h
//

#ifndef _MIDI_H_
#define _MIDI_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct
{
    uint8_t note_status;
    uint32_t note;
    uint32_t velocity;
} midi_t;

void * midi_thread();
int midi_init();
midi_t get_midi_data();
double get_note_amplitude(const int velocity);

#endif // _MIDI_H_
