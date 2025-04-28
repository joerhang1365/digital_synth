// oscillator.c
//

#include <stdint.h>
#include "oscillator.h"

// INTERNEL FUNCTION DECLARATIONS
//

static double square_wave(double * phase, double amp, double freq);
static double sawtooth_wave(double * phase, double amp, double freq);
static double sine_wave(double * phase, double amp, double freq);
int wave_list_size();

// INTERNAL VARIABLE DECLARATIONS
//

static struct wave * wave_list = NULL;

// EXTERN FUNCTION DEFINITIONS
//

void oscillator(double * left_out, double * right_out)
{
    struct wave * target;
    double sample;
    uint32_t size;

    if (wave_list == NULL)
        return;

    target = wave_list;
    sample = 0.0f;
    size = wave_list_size();

    // get the sum of all the waves
    while (target != NULL)
    {
        switch (target->type)
        {
        case SQUARE:
            target->left_out = square_wave(&target->phase, target->amp, target->freq);
            target->right_out = target->left_out;
            break;
        case SAWTOOTH:
            target->left_out = sawtooth_wave(&target->phase, target->amp, target->freq);
            target->right_out = target->left_out;
            break;
        case SINE:
            target->left_out = sine_wave(&target->phase, target->amp, target->freq);
            target->right_out = target->left_out;
            break;
        }

        sample += target->left_out;
    }

    // normalize wave [0, 1]
    if (size > 0)
        sample = (double) sample / size;
    else
        sample = 0.0f;

    // set outputs
    *left_out = sample;
    *right_out = sample;
}

// adds wave to tail of linked list
void create_wave(enum wave_type type, const float freq,
    const float amp)
{
    // check if list is empty
    if (wave_list == NULL)
    {
        wave_list = (struct wave *)malloc(sizeof(struct wave));
        if (wave_list == NULL)
        {
            printf("ERROR: cannot allocate memory to new wave\n");
            return;
        }

        wave_list->type = type;
        wave_list->freq = freq;
        wave_list->amp = amp;
        wave_list->phase = 0.0f;
        wave_list->next = NULL;
        return;
    }

    // if the head is not empty travel through linked list to tail &
    // check if frequency already exists
    struct wave * target = wave_list;

    while (target->next != NULL)
    {
        if (target->freq == freq)
            return;

        target = target->next;
    }

    if (target->freq == freq)
        return;

    struct wave * temp = (struct wave *)malloc(sizeof(struct wave));
    if (temp == NULL)
    {
        printf("ERROR: cannot allocate memory to new wave\n");
        return;
    }

    temp->type = type;
    temp->freq = freq;
    temp->amp = amp;
    temp->phase = 0.0f;
    temp->next = NULL;

    target->next = temp;
}

double get_note_frequency(const int note)
{
    return 440.0F * pow(2.0f, (note - 69) / 12.0f);
}

// INTERNAL FUNCTION DEFINITIONS
//

static double square_wave(double * phase, double amp, double freq)
{
    double output;

    output = (sin(*phase) >= 0) ? (1.0f * amp) : (-1.0f * amp);
    *phase += M_PI2 * freq * SAMPLE_RATE_INVERSE;

    if (*phase >= M_PI2)
        *phase -= M_PI2;

    return output;
}

static double sawtooth_wave(double * phase, double amp, double freq)
{
    double output;

    output = *phase * amp;
    *phase += freq * SAMPLE_RATE_INVERSE;

    if (*phase >= 1.0f)
        *phase -= 2.0f;

    return output;
}

static double sine_wave(double * phase, double amp, double freq)
{
    double output;

    output = sin(*phase) * amp;
    *phase += M_PI2 * freq * SAMPLE_RATE_INVERSE;

    if (*phase >= M_PI2)
        *phase -= M_PI2;

    return output;
}

void destroy_wave(const float freq)
{
    struct wave * prev = NULL;
    struct wave * curr = wave_list;

    while (curr != NULL)
    {
        if (curr->freq == freq)
            break;

        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) return;

    if (curr == wave_list)
    {
        wave_list = curr->next;
    }
    else
    {
        prev->next = curr->next;
    }

    free (curr);
    curr = NULL;
}

void destroy_all_waves()
{
    struct wave * temp;

    while (wave_list != NULL)
    {
        temp = wave_list->next;
        free(wave_list);
        wave_list = temp;
    }
}

int wave_list_size()
{
    struct wave * target = wave_list;
    int count = 0;

    while (target != NULL)
    {
        count++;
        target = target->next;
    }

    return count;
}
