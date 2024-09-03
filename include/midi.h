#ifndef MIDI_H
#define MIDI_H

#include <stdio.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>        /* for midi input interpreter thread */

typedef struct
{
  unsigned char note_status;
  unsigned int note;
  unsigned int velocity;
} midi_data;

void *midiThread();
int midiInit();
midi_data getMidiData();

#endif
