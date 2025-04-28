// midi.c
//

#include <linux/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
#include "error.h"

#include <pthread.h>        /* for midi input interpreter thread */

#include "midi.h"

// MIDI Protocol
// value (dec)  value (hex)   cmd       data bytes
// -----------------------------------------------------------------------
// 128-143      80-8F         note off  2 (note, velocity)
// 144-159      90-9F         note on   2 (note, velocity)
// 160-175      A0-AF         key psr   2 (note, key pressure)
// 176-191      B0-BF         ctrl chng 2 (controller no., value)
// 192-207      C0-DF         prog chng 1 (program no.)
// 208-223      D0-DF         chnl psr  1 (pressure)
// 224-239      E0-EF         pitch bnd 2 (least sig. byte, most sig. byte)

// INTERNAL CONSTANT DEFINITIONS
//

#define MAX_VELOCITY_INVERSE (double) 1.0f / 127
#define NOTE_OFF 0
#define NOTE_ON 1

// INTERNAL VARIABLES
//

static midi_t data;
static uint32_t seqfd;            // sequencer file descriptor
static uint8_t midi_initialized;

static pthread_t midi_in_thread;  // thread blocker for midi input

// EXPORTED FUNCTION DEFINITIONS
//

int midi_init(void)
{
    int status;
    char midi_path[20];

    // print all devices in /dev so the user can see and enter the path
    system("ls /dev");
    printf("[ list of devices at \"/dev\" ]\n");
    printf("[ enter midi device path EX: \"/dev/midi1\" ]:");

    status = scanf("%s", midi_path);
    if (status != 1)
    {
        printf("ERROR: invalid input for midi device path\n");
        return -EINVAL;
    }

    // first open the sequencer device for reading
    seqfd = open(midi_path, O_RDONLY);
    if (seqfd < 0)
    {
        printf("ERROR: cannot open %s\n", midi_path);
        return -EINVAL;
    }

    status = pthread_create(&midi_in_thread, NULL, midi_thread, NULL);
    if (status)
    {
        printf("ERROR: unable to create MIDI input thread\n");
        return -EINVAL;
    }

    midi_initialized = 1;

    return 0;
}

void * midi_thread()
{
    char inbytes[3];
    int status;

    // wait around for MIDI bytes to arrive and print them to screen
    while (midi_initialized)
    {
        status = read(seqfd, &inbytes, sizeof(inbytes));
        if (status <= 0)
            continue; // no bytes being sent go back to top

        // for some reason note off and note on share the same HEX code so
        // need to check if note is already on to turn it off
        // I guess some keyboards use the same hex value but just say velocity
        // is 0 so I will check that too

        char code = inbytes[0];
        char note = inbytes[1];
        char velocity = inbytes[2];

        if ((code >= 0x80 && code <= 0x8F) ||
            (code >= 0x90 && code <= 0x9F && velocity == 0))
        {
            // note off
            data.note_status = NOTE_OFF;
            data.note = note;
            data.velocity = velocity;
        }
        else if (code >= 0x90 && code <= 0x9F)
        {
            // note on
            data.note_status = NOTE_ON;
            data.note = note;
            data.velocity =  velocity;
        }
    }

    return 0;
}

midi_t get_midi_data()
{
    return data;
}

double get_note_amplitude(const int velocity)
{
    return velocity * MAX_VELOCITY_INVERSE;
}
