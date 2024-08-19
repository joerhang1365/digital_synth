#include "stdio.h"
#include "portaudio.h"

typedef int pa_stream_callback(const void *input,
                             void *output,
                             unsigned long frame_count,
                             const PaStreamCallbackTimeInfo* time_info,
                             PaStreamCallbackFlags status_flags,
                             void *user_data);

#define SAMPLE_RATE 44100
#define NUM_SECONDS 5

typedef struct
{
  float left_phase;
  float right_phase;
} paTestData;

static paTestData data;
static int pa_version;

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/ 
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    /* Cast data passed through stream to our structure. */
    paTestData *data = (paTestData*)userData; 
    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */
    
    for( i=0; i<framesPerBuffer; i++ )
    {
         *(out++) = data->left_phase;  /* left */
         *(out++) = data->right_phase;  /* right */
        /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. */
        data->left_phase += 0.01f;
        /* When signal reaches top, drop back down. */
        if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
        /* higher pitch so we can distinguish left and right. */
        data->right_phase += 0.03f;
        if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
        printf("%f\n", *out);
    }
    return 0;
}

int main(int argc, char** argv)
{
  PaStream *stream;
  PaError error;

  /* initialize PortAudio */
  error = Pa_Initialize();
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));

  /* get and print PortAudio version */
  pa_version = Pa_GetVersion();
  printf("%s\n", Pa_GetVersionText());

  /* open an audio I/O stream */
  error = Pa_OpenDefaultStream(&stream,
                               0,         /* no input channels */
                               2,         /* stereo output */
                               paFloat32, /* 32 bit floating point output */
                               SAMPLE_RATE,
                               256, /* frames per buffer or the number of sample frames
                                       that PortAudio will request from callback */
                               patestCallback,
                               &data); /* pointer to be passed to callback function */
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));

  error = Pa_StartStream(stream);
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));

  /* sleep for a few seconds */
  Pa_Sleep(NUM_SECONDS * 1000);

  error = Pa_StopStream(stream);
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));

  error = Pa_CloseStream(stream);
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));

  error = Pa_Terminate();
  if (error != paNoError)
    printf("PortAudio error: %s\n", Pa_GetErrorText(error));
}
