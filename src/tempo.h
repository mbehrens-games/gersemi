/*******************************************************************************
** tempo.h (tempo constants)
*******************************************************************************/

#ifndef TEMPO_H
#define TEMPO_H

#define TEMPO_BPM_LOWER_BOUND 32
#define TEMPO_BPM_UPPER_BOUND 255

#define TEMPO_NUM_BPMS (TEMPO_BPM_UPPER_BOUND - TEMPO_BPM_LOWER_BOUND + 1)

#define TEMPO_BPM_IS_VALID(tempo)                                              \
  ((tempo >= TEMPO_BPM_LOWER_BOUND) && (tempo <= TEMPO_BPM_UPPER_BOUND))

#define TEMPO_BPM_IS_NOT_VALID(tempo)                                          \
  (!(TEMPO_BPM_IS_VALID(tempo)))

#define TEMPO_SWING_LOWER_BOUND 1
#define TEMPO_SWING_UPPER_BOUND 8

#define TEMPO_NUM_SWINGS (TEMPO_SWING_UPPER_BOUND - TEMPO_SWING_LOWER_BOUND + 1)

/* beats / second = (beats / minute) * (1 minute / 60 seconds)    */
/* so, we multiply the tempo (in bpm) by 1/60 = 0.016666666666667 */
#define TEMPO_COMPUTE_BEAT_FREQUENCY(tempo)                                    \
  (tempo * 0.016666666666667f)

/* with 1:1 swing, each half beat's period is 1/2 of the beat period  */
/* with p:q swing, the beat is divided into (p + q) intervals, each   */
/*   interval having a period of 1/(p + q) of the beat period.        */
/*   which is determined by: px + qx = 1 -> x = 1/(p + q).            */

/* so, the 1st half beat's multiplier is: (p/(p + q))/(1/2) = 2p/(p + q) */
#define TEMPO_COMPUTE_SWING_MULTIPLIER(p, q)                                   \
  ((2.0f * p) / (p + q))

#endif
