#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "portaudio.h"
#include "globals.h"
#include "oscillator.h"
#include "notes.h"
#include "midi.h"

#define PA_SLEEP_DURATION FRAMES_PER_BUFFER * SAMPLE_RATE_INVERSE * 1000

static void printSupportedStandardSampleRates(const PaStreamParameters *input_parameters,
                                              const PaStreamParameters *output_parameters)
{
  static double standard_sample_rates[] = 
  {
    8000.0f, 9600.0f, 12000.0f, 16000.0f, 22050.0f, 24000.0f, 32000.0f,
    44100.0f, 48000.0f, 88300.0f, 96000.0f, 192000.0f, -1 /* negative terminated list */
  };

  int print_count = 0;
  PaError error;

  for (int i = 0; standard_sample_rates[i] > 0; i++)
  {
    error = Pa_IsFormatSupported(input_parameters, output_parameters, standard_sample_rates[i]);
    if (error == paFormatIsSupported)
    {
      if (print_count == 0)
      {
        printf("\t%8.2f", standard_sample_rates[i]);
      }
      else if (print_count == 4)
      {
        printf(",\n\t%8.2f", standard_sample_rates[i]);
      }
      else 
      {
        printf(", %8.2f", standard_sample_rates[i]);
      }
    }
  }

  if (!print_count) printf("none\n");
  else printf("\n");
}

/* This routine will be called by the PortAudio engine when audio is needed.
 * It may called at interrupt level on some machines so don't do anything
 * that could mess up the system like calling malloc() or free().
 */
static int paCallback(const void *input_buffer, void *output_buffer,
                      unsigned long frames_per_buffer,
                      const PaStreamCallbackTimeInfo* time_info,
                      PaStreamCallbackFlags status_flags,
                      void *user_data )
{
  /* create variables with set types */
  //voice **head = (voice**) user_data;
  //float *out = (float*) output_buffer;
  (void) time_info; /* Prevent unused variable warning */
  (void) status_flags;
  (void) input_buffer;
    
  for(unsigned int i = 0; i < frames_per_buffer; i++)
  { 
    /*if ((*head) == NULL) continue;
    
    double sample = 0.0f;

    while ((*head) != NULL)
    {
      oscillator((*head));
      sample += (*head)->left_out;
      head = &((*head)->next);
    }

    (*out++) = (double) sample / 2;
    (*out++) = (double) sample / 2;
    printf("%f\n", sample/2);
  */} 

  return paContinue;
}

static void endProgram(const int type)
{
  Pa_Terminate();

  exit(type);
}

static void paCheckError(const PaError err)
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
  const PaDeviceInfo *device_info;
  PaStream *stream;
  PaError error;
  int user_device;
  int num_devices;
  int default_displayed;
  voice *voice_head = NULL;

  /* initialize PortAudio */
  error = Pa_Initialize();
  paCheckError(error);

  /* print PortAudio version */
  printf("PortAudio version: 0x%08X\n", Pa_GetVersion());
  printf("version text: '%s'\n", Pa_GetVersionInfo()->versionText);

  /* find audio device */
  num_devices = Pa_GetDeviceCount();
  if (num_devices < 0)
  {
    printf("Error Pa_CountDevices returned 0x%x\n", num_devices);
    error = num_devices;
    paCheckError(error);
  }
  printf("number of devices: %d\n", num_devices);

  for (int i = 0; i < num_devices; i++)
  {
    device_info = Pa_GetDeviceInfo(i);
    printf("DEVICE %d: ----------------------------------------------\n", i);

    /* mark global and API specific default devices */
    default_displayed = 0;
    if (i == Pa_GetDefaultInputDevice())
    {
      printf("[ default input");
      default_displayed = 1;
    }
    else if(i == Pa_GetHostApiInfo(device_info->hostApi)->defaultInputDevice)
    {
      const PaHostApiInfo *host_info = Pa_GetHostApiInfo(device_info->hostApi);
      printf("[ default %s input", host_info->name);
      default_displayed = 1;
    }
    if (i == Pa_GetDefaultOutputDevice())
    {
      printf((default_displayed ? "," : "["));
      printf(" default ouput");
      default_displayed = 1;
    }
    else if(i == Pa_GetHostApiInfo(device_info->hostApi)->defaultOutputDevice)
    {
      const PaHostApiInfo *host_info = Pa_GetHostApiInfo(device_info->hostApi);
      printf((default_displayed ? "," : "["));
      printf(" default %s ouput", host_info->name);
    }

    if (default_displayed) printf(" ]\n");

    /* print device info fields */
    printf("name                          = %s\n", device_info->name);
    printf("host API                      = %s\n", Pa_GetHostApiInfo(device_info->hostApi)->name);
    printf("max input channels            = %d\n", device_info->maxInputChannels);
    printf("max output channels           = %d\n", device_info->maxOutputChannels);
    printf("default sample rate           = %8.2f\n", device_info->defaultSampleRate);
  }

  printf("select a device: ");
  int check = scanf("%d", &user_device);

  if (check != 1)
  {
    printf("incorrect device input\n");
    endProgram(EXIT_SUCCESS);
  }
  else if (user_device > num_devices || user_device < 0)
  {
    printf("selected device does not exist\n");
    endProgram(EXIT_SUCCESS);
  }

  device_info = Pa_GetDeviceInfo(user_device);

  /* set input and output parameters */
  //memset(&input_parameters, 0, sizeof(input_parameters));
  input_parameters.channelCount = device_info->maxInputChannels;
  input_parameters.device = user_device;
  input_parameters.hostApiSpecificStreamInfo = NULL;
  input_parameters.sampleFormat = paFloat32;
  input_parameters.suggestedLatency = device_info->defaultLowInputLatency;

  //memset(&output_parameters, 0, sizeof(output_parameters));
  output_parameters.channelCount = device_info->maxInputChannels; 
  output_parameters.device = user_device;
  output_parameters.hostApiSpecificStreamInfo = NULL;
  output_parameters.sampleFormat = paFloat32;
  output_parameters.suggestedLatency = device_info->defaultLowInputLatency;

  if (input_parameters.channelCount > 0)
  {
    printf("supported standard sample rates\n for half-duplex 16 bit %d channel input = \n",
           input_parameters.channelCount);
    printSupportedStandardSampleRates(&input_parameters, NULL);
  }
  if (output_parameters.channelCount > 0)
  {
    printf("supported standard sample rates\n for half-duplex 16 bit %d channel output = \n",
           output_parameters.channelCount);
    printSupportedStandardSampleRates(NULL, &output_parameters);
  }
  if (input_parameters.channelCount > 0 && output_parameters.channelCount > 0)
  {
    printf("supported standard sample rates\n for half-duplex 16 bit %d channel input, %d channel output",
           input_parameters.channelCount, output_parameters.channelCount);
    printSupportedStandardSampleRates(&input_parameters, &output_parameters);
  }
 
  /* open an audio I/O stream */
  error = Pa_OpenStream(&stream,
                        &input_parameters,
                        &output_parameters,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paNoFlag, /* can define dither, clip settings or other */
                        paCallback,
                        &voice_head); /* pointer to be passed to callback function */

  paCheckError(error);

  error = Pa_StartStream(stream);
  paCheckError(error);

  notesInit(); 
  midiInit();

  addVoice(&voice_head, SINE, getNote(69), velocityToAmplitude(50));
  addVoice(&voice_head, SINE, getNote(60), velocityToAmplitude(50));

  /* start of program loop */
  while (1)
  {
    midi_data midi_data = getMidiData();

    if (midi_data.note_status == 1)
    {
      addVoice(&voice_head, SINE, getNote(midi_data.note + 3), velocityToAmplitude(midi_data.velocity));
    }
    
    Pa_Sleep(PA_SLEEP_DURATION);
  }

  /* end of program loop */

  error = Pa_StopStream(stream);
  paCheckError(error);

  error = Pa_CloseStream(stream);
  paCheckError(error);
  endProgram(EXIT_SUCCESS);
}
