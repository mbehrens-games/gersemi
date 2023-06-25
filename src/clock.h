/*******************************************************************************
** clock.h (clock rates)
*******************************************************************************/

#ifndef CLOCK_H
#define CLOCK_H

/* the sampling rate is set to be high enough so  */
/* that C10 (the highest pitch in the frequency   */
/* table) is under the Nyquist.                   */

/* additionally, we want the sampling rate to be  */
/* a multiple of 1000, so each millisecond has an */
/* integer number of samples.                     */

/* in equal temperament, the tuning forks from C256 to  */
/* A444 give a range for C10 of 16384 to 16896.         */
/* (C10 is the top note in the tuning table.)           */

#define CLOCK_SAMPLING_RATE        32000

/* the delta_t's are obtained using */
/* the equation delta_t = 1 / f_s,  */
/* where f_s is the sampling rate.  */

#define CLOCK_DELTA_T_NANOSECONDS  31250
#define CLOCK_DELTA_T_SECONDS      0.000031250f

/* the 1hz phase increment is found by dividing the   */
/* size of the 28 bit phase register by the sampling  */
/* rate: 2^28 / f_s = 268435456 / 32000 = 8388        */

#define CLOCK_1HZ_PHASE_INCREMENT  8388.608f

#endif
