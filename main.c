#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"

#define SAMPLE_RATE 44100
#define SAMPLE_RATE_INVERSE (double) 1 / SAMPLE_RATE
#define CHANNEL_COUNT 2 /* stereo */
#define FRAMES_PER_BUFFER 64 /* the number of sample frames per callback */
#define PA_SLEEP_DURATION FRAMES_PER_BUFFER * SAMPLE_RATE_INVERSE * 1000
#ifndef M_PI
#define M_PI 3.14159265
#endif
#define NOTES_NUM 88

enum wave_type { SINE, SQUARE, SAWTOOTH };

typedef struct
{
  enum wave_type type;
  double left_phase;
  double right_phase;
  double phase_offset;
  double frequency;
  double amplitude;
} 
wave_data;

static double notes[NOTES_NUM];

/* calculate hertz for each note
 * note A is notes[48] which is 440 hertz
 */
void notesInit()
{
  for (int i = 0; i < NOTES_NUM; i++)
  {
    notes[i] = 440.0f * pow(2.0f, (i - 48) / 12.0f);
  }
}

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
*/ 
static int paCallback(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData )
{
  /* create variables with set types */
  wave_data *data = (wave_data*) userData;
  float *out = (float*) outputBuffer;
  (void) timeInfo; /* Prevent unused variable warning */
  (void) statusFlags;
  (void) inputBuffer;
    
  for(unsigned int i = 0; i < framesPerBuffer; i++)
  {
    if (data->type == SINE)
    {
      *(out++) = data->left_phase * data->amplitude; /* left */
      *(out++) = data->right_phase * data->amplitude; /* right */

      data->left_phase = sin(data->frequency * SAMPLE_RATE_INVERSE * i);
      data->right_phase = sin(data->frequency * SAMPLE_RATE_INVERSE * i); 
    }
    else if (data->type == SQUARE)
    {
      if (data->left_phase < 0.5f) *(out++) = -1.0f; /* left */
      else *(out++) = 1.0f * data->amplitude;

      if (data->right_phase < 0.5f) *(out++) = -1.0f; /* right */
      else *(out++) = 1.0f * data->amplitude;

      data->left_phase += data->frequency * SAMPLE_RATE_INVERSE;
      if (data->left_phase >= 1.0f) data->left_phase -= 1.0f;

      data->right_phase += data->frequency * SAMPLE_RATE_INVERSE + data->phase_offset;
      if (data->right_phase >= 1.0f) data->right_phase = -1.0f;
    }
    else if (data->type == SAWTOOTH)
    {
      *(out++) = data->left_phase * data->amplitude; /* left */
      *(out++) = data->right_phase * data->amplitude; /* right */

      data->left_phase += data->frequency * SAMPLE_RATE_INVERSE;
      if (data->left_phase >= 1.0f) data->left_phase -= 2.0f;
      data->right_phase += data->frequency * SAMPLE_RATE_INVERSE + data->phase_offset;
      if (data->right_phase >= 1.0f) data->right_phase -= 2.0f;
    }
    
    printf("%f", *out);
  }

  return paContinue;
}

static void endProgram(int type)
{
  Pa_Terminate();

  exit(type);
}

static void paCheckError(PaError err)
{
  if (err != paNoError)
  {
    printf("PortAudio ERROR: %s\n", Pa_GetErrorText(err));

    endProgram(EXIT_FAILURE);
  }
}

int main(int argc, char** argv)
{
  PaStreamParameters output_parameters;
  PaStreamParameters input_parameters;
  PaStream *stream;
  PaError error;
  int user_device;
  wave_data data;

  /* initialize PortAudio */
  error = Pa_Initialize();
  paCheckError(error);

  /* print PortAudio version */
  printf("%s\n", Pa_GetVersionText());

  /* find audio device */
  int num_devices;

  num_devices = Pa_GetDeviceCount();
  if (num_devices < 0)
  {
    printf("Error Pa_CountDevices returned 0x%x\n", num_devices);
    error = num_devices;
    paCheckError(error);
  }
  else if (num_devices == 0)
  {
    printf("There are no devices connected\n");
    endProgram(EXIT_SUCCESS);
  }
  else 
  {
    printf("Number of devices: %d\n", num_devices);
  }

  const PaDeviceInfo *device_info;
  for (int i = 0; i < num_devices; i++)
  {
    device_info = Pa_GetDeviceInfo(i);
    printf("device %d:\n", i);
    printf("  name: %s:\n", device_info->name);
    printf("  max input channels: %d\n", device_info->maxInputChannels);
    printf("  max output channels: %d\n", device_info->maxOutputChannels);
    printf("  default sample rate: %lf\n", device_info->defaultSampleRate);
  }

  printf("Select a device: ");
  int check = scanf("%d", &user_device);

  if (check != 1)
  {
    printf("Incorrect device input\n");
    endProgram(EXIT_SUCCESS);
  }
  else if (user_device > num_devices || user_device < 0)
  {
    printf("Selected device does not exist\n");
    endProgram(EXIT_SUCCESS);
  }

  /* set input and output parameters */
  memset(&input_parameters, 0, sizeof(input_parameters));
  input_parameters.channelCount = CHANNEL_COUNT; // left and right
  input_parameters.device = user_device;
  input_parameters.hostApiSpecificStreamInfo = NULL;
  input_parameters.sampleFormat = paFloat32;
  input_parameters.suggestedLatency = 
    Pa_GetDeviceInfo(user_device)->defaultLowInputLatency;

  memset(&output_parameters, 0, sizeof(output_parameters));
  output_parameters.channelCount = CHANNEL_COUNT; // left and right
  output_parameters.device = user_device;
  output_parameters.hostApiSpecificStreamInfo = NULL;
  output_parameters.sampleFormat = paFloat32;
  output_parameters.suggestedLatency = 
    Pa_GetDeviceInfo(user_device)->defaultLowInputLatency;
 
  /* open an audio I/O stream */
  error = Pa_OpenStream(&stream,
                        &input_parameters,
                        &output_parameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paNoFlag, /* can define dither, clip settings or other */
                        paCallback,
                        &data); /* pointer to be passed to callback function */

  paCheckError(error);

  error = Pa_StartStream(stream);
  paCheckError(error);

  notesInit(); 

  /* initialize wave_data */
  data.type = SINE;
  data.frequency = notes[48]; /* A4 index 48*/
  data.amplitude = 1.0f;
  data.left_phase = 0.0f;
  data.right_phase = 0.0f;
  data.phase_offset = 0.00f; 

  /* start of program loop */
  while (1)
  { 
    Pa_Sleep(PA_SLEEP_DURATION);
  }

  /* end of program loop */

  error = Pa_StopStream(stream);
  paCheckError(error);

  error = Pa_CloseStream(stream);
  paCheckError(error);

  endProgram(EXIT_SUCCESS);
}
