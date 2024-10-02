#ifndef OSCILLATOR_H
#define OSCILLATOR_H

/* i am going to do something crazy because i feel like it
 * instead of looping through an array and seeing which waves are active
 * i am going to create a linked list of active waves then find the average
 * of all the waves to output to speakers */

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include <math.h>

enum wave_type { SINE, SQUARE, SAWTOOTH };

typedef struct wave
{
  enum wave_type type;
  double phase;
  double frequency;
  double amplitude;
  double left_out;
  double right_out;

  struct wave *next;
} wave;

void oscillator(double *left_out, double *right_out);
void createWave(enum wave_type type, const float frequency, const float amplitude);
void destroyWave(const float frequency);
void destroyAllWaves();
int waveListSize();

/* returns the frequency of an input note 
 * the base note is A4 which is note 69 on midi */
double frequency(const int note);

#endif
