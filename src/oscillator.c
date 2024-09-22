#include "oscillator.h"

void oscillator(voice *data)
{
  if (data->type == SINE)
  {
    data->left_out = sin(data->phase) * data->amplitude; /* left */
    data->right_out = sin(data->phase) * data->amplitude; /* right */

    data->phase += M_PI_2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->phase >= M_PI_2) data->phase -= M_PI_2;
  }
  else if (data->type == SQUARE)
  {
    data->left_out = sin(data->phase >= 0) ? 1.0f * data->amplitude : -1.0f * data->amplitude;
    data->right_out = sin(data->phase) >= 0 ? 1.0f * data->amplitude : -1.0f * data->amplitude;

    data->phase += M_PI_2 * data->frequency * SAMPLE_RATE_INVERSE;
    if (data->phase >= M_PI_2) data->phase -= M_PI_2;
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

/* adds voice to tail of linked list
 */
void addVoice(voice **head, 
               enum wave_type type, 
               const float frequency, 
               const float amplitude)
{
  while (*head != NULL)
  {
    head = &((*head)->next);
  }

  voice *temp = (voice*)malloc(sizeof(voice));
  if (temp == NULL)
  {
    printf("ERROR: allocating memory to new voice\n");
    return;
  }

  temp->type = type;
  temp->frequency = frequency;
  temp->amplitude = amplitude;
  temp->phase = 0.0f;

  *head = temp;
}

void removeVoice(voice **head, const float frequency)
{
  voice *previous;
  voice *current;

  current = *head;
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

int getVoicesSize(voice **head)
{
  int count = 0;
  while (*head != NULL)
  {
    count++;
    head = &((*head)->next);
  }

  return count;
}
