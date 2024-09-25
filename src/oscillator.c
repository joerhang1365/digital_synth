#include "oscillator.h"

void oscillator(wave *data)
{
  if (data->type == SINE)
  {
    data->left_out = sin(data->phase) * data->amplitude; /* left */
    data->right_out = sin(data->phase) * data->amplitude; /* right */

    data->phase += M_PI2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->phase >= M_PI2) data->phase -= M_PI2;
  }
  else if (data->type == SQUARE)
  {
    data->left_out = sin(data->phase) >= 0 ? 1.0f * data->amplitude : -1.0f * data->amplitude;
    data->right_out = sin(data->phase) >= 0 ? 1.0f * data->amplitude : -1.0f * data->amplitude;

    data->phase += M_PI2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->phase >= M_PI2) data->phase -= M_PI2;
  }
  else if (data->type == SAWTOOTH)
  {
    data->left_out = data->phase * data->amplitude; /* left */
    data->right_out = data->phase * data->amplitude; /* right */

    data->phase += data->frequency * SAMPLE_RATE_INVERSE;
    if (data->phase >= 1.0f) data->phase -= 2.0f;
  }  
  //printf("%f\n", *out);
}

/* adds wave to tail of linked list */
void addWave(wave **head, 
               enum wave_type type, 
               const float frequency, 
               const float amplitude)
{
  /* travel through linked list to tail & 
   * check if frequency already exists */
  while (*head != NULL)
  {
    if ((*head)->frequency == frequency) return;
    head = &((*head)->next);
  }

  wave *temp = (wave*)malloc(sizeof(wave));
  if (temp == NULL)
  {
    printf("ERROR: cannot allocate memory to new voice\n");
    return;
  }

  temp->type = type;
  temp->frequency = frequency;
  temp->amplitude = amplitude;
  temp->phase = 0.0f;
  temp->next = NULL;

  *head = temp;
}

void removeWave(wave **head, const float frequency)
{
  wave *previous = NULL;
  wave *current = *head;

  while (current != NULL)
  {
    if (current->frequency == frequency) break;

    previous = current;
    current = current->next;
  }

  if (current == NULL) return;

  if (current == *head)
  {
    *head = current->next;
  }
  else 
  {
    previous->next = current->next;
  }

  free (current);
}

int waveListSize(wave **head)
{
  int count = 0;
  while (*head != NULL)
  {
    count++;
    head = &((*head)->next);
  }

  return count;
}

double frequency(const int note)
{
  return 440.0F * pow(2.0f, (note - 69) / 12.0f);
}
