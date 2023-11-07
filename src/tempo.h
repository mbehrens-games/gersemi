/*******************************************************************************
** tempo.h (tempo)
*******************************************************************************/

#ifndef TEMPO_H
#define TEMPO_H

/* the tempo values are in bpm */
#define TEMPO_DEFAULT       120
#define TEMPO_LOWER_BOUND    40
#define TEMPO_UPPER_BOUND   240
#define TEMPO_NUM_VALUES    (TEMPO_UPPER_BOUND - TEMPO_LOWER_BOUND + 1)

#define TEMPO_COMPUTE_BEATS_PER_SECOND(tempo)                                  \
  ((tempo) / 60.0f)

#define TEMPO_COMPUTE_SECONDS_PER_BEAT(tempo)                                  \
  (60.0f / (tempo))

#endif
