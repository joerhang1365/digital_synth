#include "midi.h"

/* MIDI Protocol
 * value (dec)  value (hex)   cmd       data bytes
 * -----------------------------------------------------------------------
 * 128-143      80-8F         note off  2 (note, velocity)
 * 144-159      90-9F         note on   2 (note, velocity)
 * 160-175      A0-AF         key psr   2 (note, key pressure)
 * 176-191      B0-BF         ctrl chng 2 (controller no., value)
 * 192-207      C0-DF         prog chng 1 (program no.)
 * 208-223      D0-DF         chnl psr  1 (pressure)
 * 224-239      E0-EF         pitch bnd 2 (least sig. byte, most sig. byte) 
 */

#define MAX_VELOCITY_INVERSE (double) 1.0f / 127

static midi_data data;
static unsigned int seqfd;          /* sequencer file descriptor */
static unsigned int is_loop;
static pthread_t midi_in_thread;    /* thread blocker for midi input */
static unsigned char inbytes[3];

int midiInit(void)
{
  int status;
  char midi_path[20];

  // get midi device
  system("ls /dev");
  printf("list of devices at /dev\n");
  printf("enter midi device path (ex: /dev/midi1):\n");
  status = scanf("%s\n", midi_path);
  if (status != 1)
  {
    printf("ERROR: invalid input for midi device path\n");
    return 1;
  }

  // first open the sequencer device for reading
  seqfd = open(midi_path, O_RDONLY);
  if (seqfd == -1)
  {
    printf("ERROR: cannot open %s\n", midi_path);
    return 1;
  }

  status = pthread_create(&midi_in_thread, NULL, midiThread, NULL);
  if (status)
  {
    printf("ERROR: unable to create MIDI input thread\n");
    return 1;
  }

  is_loop = 1;

  return 0;
}

void *midiThread()
{
  int status;

  status = pthread_join(midi_in_thread, NULL);
  if (status)
  {
    printf("ERROR: unable to join MIDI input thread\n");
    //return 0;
  }

  // now just wait around for MIDI bytes to arrive and print them to screen
  while (is_loop)
  {
    status = read(seqfd, &inbytes, sizeof(inbytes));
    if (status <= 0) continue; // no bytes being sent go back to top

    // note off
    if (inbytes[0] >= 0x80 && inbytes[0] <= 0x8F)
    {
      data.note_status = 0;
      data.note = inbytes[1];
      data.velocity = inbytes[2];
    }
    // note on
    else if (inbytes[0] >= 0x90 && inbytes[0] <= 0x9F)
    {
      data.note_status = 1;
      data.note = inbytes[1];
      data.velocity = inbytes[2];

      if (data.velocity != 0) printf("note: %d  velocity: %d\n", inbytes[1], inbytes[2]);
    } 
  }

  return 0;
}

midi_data getMidiData()
{
  return data;
}

double velocityToAmplitude(int velocity)
{
  return velocity * MAX_VELOCITY_INVERSE;
}
