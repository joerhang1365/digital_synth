// oscillator.h
//

#ifndef _OSCILLATOR_H_
#define _OSCILLATOR_H_

// i am going to do something crazy because i feel like it
// instead of looping through an array and seeing which waves are active
// i am going to create a linked list of active waves then find the average
// of all the waves to output to speakers

#include <stdio.h>
#include <stdlib.h>
#include "conf.h"
#include <math.h>

enum wave_type {
    SINE,
    SQUARE,
    SAWTOOTH
};

struct wave
{
    enum wave_type type;
    double phase;
    double freq;
    double amp;
    double left_out;
    double right_out;

    struct wave * next;
};

void oscillator(double * left_out, double * right_out);
void create_wave(enum wave_type type, const float frequency, const float amplitude);
void destroy_wave(const float freq);
void destroy_all_waves();

// returns the frequency of an input note
// the base note is A4 which is note 69 on midi
double get_note_frequency(const int note);

#endif // _OSCILLATOR_H_
