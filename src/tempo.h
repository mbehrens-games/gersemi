/*******************************************************************************
** tempo.h (tempo)
*******************************************************************************/

#ifndef TEMPO_H
#define TEMPO_H

#define TEMPO_DEFAULT     (120 - 32)      /* 120 bpm */
#define TEMPO_NUM_VALUES  (255 - 32 + 1)  /* range is 32 bpm to 255 bpm */

#define TEMPO_COMPUTE_BEATS_PER_SECOND(tempo)                                  \
  (((tempo) + 32) / 60.0f)

#define TEMPO_COMPUTE_SECONDS_PER_BEAT(tempo)                                  \
  (60.0f / ((tempo) + 32))

#endif
