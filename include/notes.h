#ifndef NOTES_H
#define NOTES_H

#include <math.h>

#define NOTES_NUM 88

/* calculate hertz for each note
 * note A4 is notes[69] which is 440 hertz
 */
void notesInit();
double getNote(unsigned int index);

#endif
