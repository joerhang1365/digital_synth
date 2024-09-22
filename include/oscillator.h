#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdio.h>
#include <stdlib.h>
#include "globals.h"
#include <math.h>

#define MAX_WAVES 128

enum wave_type { SINE, SQUARE, SAWTOOTH };

typedef struct voice
{
  enum wave_type type;
  double phase;
  double frequency;
  double amplitude;
  double left_out;
  double right_out;

  struct voice *next;
} voice;

void oscillator(voice *data);
void addVoice(voice **head, enum wave_type type, const float frequency, const float amplitude);
void removeVoice(voice **head, const float frequency);
int getVoicesSize(voice **head);

#endif
