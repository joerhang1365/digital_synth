#include "oscillator.h"

static wave *head = NULL;

void oscillator(double *left_out, double *right_out)
{
  // check if head is not NULL cuz it will fuck itself
  if (head == NULL) return;

  wave *current = head;
  double sample = 0.0f;
  const int listSize = waveListSize();
  //printf("%d\n", listSize);

  /* get the sum of all the waves */
  while (current != NULL)
  {
    //printf("%f\n", current->frequency);
    /* sinusodial wave */
    if (current->type == SINE)
    {
      current->left_out = sin(current->phase) * current->amplitude; /* left */
      current->right_out = sin(current->phase) * current->amplitude; /* right */

      current->phase += M_PI2 * current->frequency * SAMPLE_RATE_INVERSE;
      if (current->phase >= M_PI2) current->phase -= M_PI2;
    }
    /* square wave */
    else if (current->type == SQUARE)
    {
      current->left_out = sin(current->phase) >= 0 ? 1.0f * current->amplitude : -1.0f * current->amplitude;
      current->right_out = sin(current->phase) >= 0 ? 1.0f * current->amplitude : -1.0f * current->amplitude;

      current->phase += M_PI2 * current->frequency * SAMPLE_RATE_INVERSE;
      if (current->phase >= M_PI2) current->phase -= M_PI2;
    }
    /* sawtooth wave */
    else if (current->type == SAWTOOTH)
    {
      current->left_out = current->phase * current->amplitude; /* left */
      current->right_out = current->phase * current->amplitude; /* right */

      current->phase += current->frequency * SAMPLE_RATE_INVERSE;
      if (current->phase >= 1.0f) current->phase -= 2.0f;
    }

    sample += current->left_out;
    current = current->next;
  }

  /* normalize wave [0, 1] */
  if (listSize > 0) sample = (double) sample / listSize;
  else sample = 0.0f;

  /* set outputs */
  *left_out = sample;
  *right_out = sample;
}

/* adds wave to tail of linked list */
void createWave(enum wave_type type, 
               const float frequency, 
               const float amplitude)
{
  /* check if list is empty */
  if (head == NULL)
  {
    head = (wave*)malloc(sizeof(wave));
    if (head == NULL)
    {
      printf("ERROR: cannot allocate memory to new wave\n");
      return;
    }

    head->type = type;
    head->frequency = frequency;
    head->amplitude = amplitude;
    head->phase = 0.0f;
    head->next = NULL;
    return;
  }

  /* if the head is not empty travel through linked list to tail & 
   * check if frequency already exists */
  wave *current = head;
  while (current->next != NULL)
  { 
    if (current->frequency == frequency) return;
    current = current->next;
  }

  if (current->frequency == frequency) return;

  wave *temp = (wave*)malloc(sizeof(wave));
  if (temp == NULL)
  {
    printf("ERROR: cannot allocate memory to new wave\n");
    return;
  }

  temp->type = type;
  temp->frequency = frequency;
  temp->amplitude = amplitude;
  temp->phase = 0.0f;
  temp->next = NULL; 

  current->next = temp;
}

void destroyWave(const float frequency)
{
  wave *previous = NULL;
  wave *current = head;

  while (current != NULL)
  {
    if (current->frequency == frequency) break;

    previous = current;
    current = current->next;
  }

  if (current == NULL) return;

  if (current == head)
  {
    head = current->next;
  }
  else 
  {
    previous->next = current->next;
  }

  free (current);
  current = NULL;
}

void destroyAllWaves()
{
  wave *temp;

  while (head != NULL)
  {
    temp = head->next;
    free(head);
    head = temp;
  }
}

int waveListSize()
{
  wave *current = head;
  int count = 0;
  while (current != NULL)
  {
    count++;
    current = current->next;
  }

  return count;
}

double frequency(const int note)
{
  return 440.0F * pow(2.0f, (note - 69) / 12.0f);
}
