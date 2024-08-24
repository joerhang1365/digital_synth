#include <linux/soundcard.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define MIDI_DEVICE "/dev/sequencer"

int main (void)
{
  unsigned char inpacket[4];

  // first open the sequencer device for reading
  int seqfd = open(MIDI_DEVICE, O_RDONLY);
  if (seqfd < 0)
  {
    printf("ERROR: cannot open %s\n", MIDI_DEVICE);
    exit(EXIT_FAILURE);
  }

  // now just wiat around for MIDI bytes to arrive and print them to screen
  while (1)
  {
    read(seqfd, &inpacket, sizeof(inpacket));

    // print the MIDI byte if this input packet contains one
    if (inpacket[0] == SEQ_MIDIPUTC)
    {
      printf("received MIDI byte: %d\n", inpacket[1]);
    }
  }

  return 0;
}
