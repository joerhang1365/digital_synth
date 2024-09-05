#include "notes.h"

static double notes[NOTES_NUM];

/* calculate hertz for each note
 * note A is notes[69] which is 440 hertz
 */
void notesInit()
{
  for (int i = 0; i < NOTES_NUM; i++)
  {
    notes[i] = 440.0F * pow(2.0f, (i - 69) / 12.0f);
  }
}

double getNote(unsigned int index)
{
  return notes[index];
}
