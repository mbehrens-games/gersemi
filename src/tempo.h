/*******************************************************************************
** tempo.h (tempo constants)
*******************************************************************************/

#ifndef TEMPO_H
#define TEMPO_H

#define TEMPO_LOWER_BOUND 24
#define TEMPO_UPPER_BOUND 220

#define TEMPO_NUM_BPMS    (TEMPO_UPPER_BOUND - TEMPO_LOWER_BOUND + 1)

#define TEMPO_IS_VALID(tempo)                                                  \
  ((tempo >= TEMPO_LOWER_BOUND) && (tempo <= TEMPO_UPPER_BOUND))

#define TEMPO_IS_NOT_VALID(tempo)                                              \
  (!(TEMPO_IS_VALID(tempo)))

#define TEMPO_COMPUTE_INDEX(tempo)                                             \
  (tempo - TEMPO_LOWER_BOUND)

/* the swing ratios are kept in the approx. range of 1:1 to 3:1 (0.5 to 0.75) */
enum
{
  TEMPO_SWING_1_1 = 0,  /* 1/2  = 0.500 */
  TEMPO_SWING_5_4,      /* 5/9  = 0.556 */
  TEMPO_SWING_4_3,      /* 4/7  = 0.571 */
  TEMPO_SWING_3_2,      /* 3/5  = 0.600 */
  TEMPO_SWING_5_3,      /* 5/8  = 0.625 */
  TEMPO_SWING_2_1,      /* 2/3  = 0.667 */
  TEMPO_SWING_5_2,      /* 5/7  = 0.714 */
  TEMPO_SWING_3_1,      /* 3/4  = 0.750 */
  TEMPO_NUM_SWINGS
};

/* with 1:1 swing, each half beat's period is 1/2 of the beat period  */
/* with p:q swing, the beat is divided into (p + q) intervals, each   */
/*   interval having a period of 1/(p + q) of the beat period.        */
/*   which is determined by: px + qx = 1 -> x = 1/(p + q).            */

/* so, the 1st half beat's period multiplier is: (p/(p + q))/(1/2) = 2p/(p + q) */
/* and thus its frequncy multiplier is: (p + q)/2p                              */
/* similarly, the 2nd half beat's frequency multiplier is: (p + q)/2q           */

#define TEMPO_COMPUTE_MULTIPLIER_FIRST_HALF_BEAT(p, q)                         \
  ((p + q) / (2.0f * p))

#define TEMPO_COMPUTE_MULTIPLIER_SECOND_HALF_BEAT(p, q)                        \
  ((p + q) / (2.0f * q))

#define TEMPO_COMPUTE_BEAT_FREQUENCY(tempo)                                    \
  (tempo * 0.016666666666667f)

#define TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(tempo, p, q)                   \
  (tempo * 0.016666666666667f * TEMPO_COMPUTE_MULTIPLIER_FIRST_HALF_BEAT(p, q))

#define TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(tempo, p, q)                  \
  (tempo * 0.016666666666667f * TEMPO_COMPUTE_MULTIPLIER_SECOND_HALF_BEAT(p, q))

#endif
