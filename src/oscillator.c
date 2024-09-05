#include "oscillator.h"

void oscillator(wave *data)
{
  if (data->type == SINE)
  {
    data->left_out = sin(data->left_phase) * data->amplitude; /* left */
    data->right_out = sin(data->right_phase) * data->amplitude; /* right */

    data->left_phase += M_PI_2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->left_phase >= M_PI_2) data->left_phase -= M_PI_2;
    data->right_phase += M_PI_2 * (data->frequency + data->phase_offset) * SAMPLE_RATE_INVERSE;
    if (data->right_phase >= M_PI_2) data->right_phase -= M_PI_2;
  }
  else if (data->type == SQUARE)
  {
    data->left_out = sin(data->left_phase >= 0) ? 1.0f * data->amplitude : -1.0f * data->amplitude;
    data->right_out = sin(data->right_phase) >= 0 ? 1.0f * data->amplitude : -1.0f * data->amplitude;

    data->left_phase += M_PI_2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->left_phase >= M_PI_2) data->left_phase -= M_PI_2;
    data->right_phase += M_PI_2 * (data->frequency + data->phase_offset) * SAMPLE_RATE_INVERSE;
    if (data->right_phase >= M_PI_2) data->right_phase -= M_PI_2;
  }
  else if (data->type == SAWTOOTH)
  {
    data->left_out = data->left_phase * data->amplitude; /* left */
    data->right_out = data->right_phase * data->amplitude; /* right */

    data->left_phase += data->frequency * SAMPLE_RATE_INVERSE;
    if (data->left_phase >= 1.0f) data->left_phase -= 2.0f;
    data->right_phase += (data->frequency + data->phase_offset) * SAMPLE_RATE_INVERSE;
    if (data->right_phase >= 1.0f) data->right_phase -= 2.0f;
  }  
  //printf("%f\n", *out);
}
