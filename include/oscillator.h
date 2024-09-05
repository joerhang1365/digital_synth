#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "globals.h"
#include <math.h>

enum wave_type { SINE, SQUARE, SAWTOOTH };

typedef struct
{
  enum wave_type type;
  double left_phase;
  double right_phase;
  double phase_offset;
  double frequency;
  double amplitude;
  double left_out;
  double right_out;
} wave;

void oscillator(wave *data);

#endif
