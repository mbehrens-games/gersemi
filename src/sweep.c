/*******************************************************************************
** sweep.c (pitch sweep, duty cycle sweep)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "pi.h"
#include "sweep.h"
#include "tempo.h"

#define SWEEP_LOWER_BOUND (0 * 12 + 9) /* A0 */
#define SWEEP_UPPER_BOUND (8 * 12 + 0) /* C8 */

/* sweep phase increment table  */
/* 1st index: tempo             */
/* 2nd index: sweep speed       */
static unsigned int S_sweep_phase_increment_table[TEMPO_NUM_BPMS][9];

/* sweep speed table                        */
/* speed 1: 1 semitone per quarter note     */
/* speed 2: 1.5 semitones per quarter note  */
/* speed 3: 2 semitones per quarter note    */
/* speed 4: 3 semitones per quarter note    */
/* speed 5: 4 semitones per quarter note    */
/* speed 6: 6 semitones per quarter note    */
/* speed 7: 7 semitones per quarter note    */
/* speed 8: 8 semitones per quarter note    */
/* speed 9: 12 semitones per quarter note   */
static float S_sweep_speed_table[9] = 
              {1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 6.0f, 7.0f, 8.0f, 12.0f};

/*******************************************************************************
** sweep_setup()
*******************************************************************************/
short int sweep_setup(sweep* sw)
{
  if (sw == NULL)
    return 1;

  /* initialize mode & speed */
  sw->mode = SWEEP_MODE_OFF;
  sw->speed = 1;

  /* initialize phase increment */
  sw->increment = 0;

  /* initialize other variables */
  sw->phase = 0;

  sw->start = 0;
  sw->target = 0;

  sw->step_coarse = 0;
  sw->step_fine = 0;

  sw->level = 0;

  return 0;
}

/*******************************************************************************
** sweep_set_mode()
*******************************************************************************/
short int sweep_set_mode(sweep* sw, int mode)
{
  if (sw == NULL)
    return 1;

  if ((mode >= 0) && (mode <= 9))
    sw->mode = mode;
  else
    sw->mode = 0;

  return 0;
}

/*******************************************************************************
** sweep_set_speed()
*******************************************************************************/
short int sweep_set_speed(sweep* sw, int tempo, int speed)
{
  if (sw == NULL)
    return 1;

  /* make sure tempo is valid */
  if ((tempo < 0) || (tempo >= TEMPO_NUM_BPMS))
    return 0;

  /* set the speed */
  if ((speed >= 1) && (speed <= 9))
    sw->speed = speed;
  else
    sw->speed = 1;

  /* set phase increment */
  sw->increment = 
    S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][speed - 1];

  return 0;
}

/*******************************************************************************
** sweep_adjust_to_tempo()
*******************************************************************************/
short int sweep_adjust_to_tempo(sweep* sw, int tempo)
{
  if (sw == NULL)
    return 1;

  /* make sure tempo is valid */
  if ((tempo < 0) || (tempo >= TEMPO_NUM_BPMS))
    return 0;

  /* set phase increment */
  sw->increment = 
    S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][sw->speed - 1];

  return 0;
}

/*******************************************************************************
** sweep_trigger()
*******************************************************************************/
short int sweep_trigger(sweep* sw, int old_note, int new_note)
{
  if (sw == NULL)
    return 1;

  /* upward sweep */
  if ((sw->mode == SWEEP_MODE_PORTAMENTO_UP)          || 
      (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_UP) || 
      (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_UP))
  {
    sw->start = new_note;
    sw->target = SWEEP_UPPER_BOUND;
  }
  /* downward sweep */
  else if ( (sw->mode == SWEEP_MODE_PORTAMENTO_DOWN)          || 
            (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_DOWN) || 
            (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_DOWN))
  {
    sw->start = new_note;
    sw->target = SWEEP_LOWER_BOUND;
  }
  /* sweep to new note (or duty cycle) */
  else if ( (sw->mode == SWEEP_MODE_PORTAMENTO_TO_NOTE)           || 
            (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_TO_NOTE)  || 
            (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_TO_NOTE))
  {
    sw->start = old_note;
    sw->target = new_note;
  }
  /* off */
  else
  {
    sw->start = new_note;
    sw->target = new_note;
  }

  /* bound start & target notes */
  if (sw->start < SWEEP_LOWER_BOUND)
    sw->start = SWEEP_LOWER_BOUND;
  else if (sw->start > SWEEP_UPPER_BOUND)
    sw->start = SWEEP_UPPER_BOUND;

  if (sw->target < SWEEP_LOWER_BOUND)
    sw->target = SWEEP_LOWER_BOUND;
  else if (sw->target > SWEEP_UPPER_BOUND)
    sw->target = SWEEP_UPPER_BOUND;

  /* initialize other variables */
  sw->phase = 0;

  sw->step_coarse = 0;
  sw->step_fine = 0;

  if ((sw->mode == SWEEP_MODE_PORTAMENTO_UP)            || 
      (sw->mode == SWEEP_MODE_PORTAMENTO_DOWN)          || 
      (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_UP)   || 
      (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_DOWN) || 
      (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_UP)  || 
      (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_DOWN))
  {
    sw->level = 0;
  }
  else if ( (sw->mode == SWEEP_MODE_PORTAMENTO_TO_NOTE)           || 
            (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_TO_NOTE)  || 
            (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_TO_NOTE))
  {
    sw->level = (sw->start - sw->target) * 64;
  }
  else
    sw->level = 0;

  return 0;
}

/*******************************************************************************
** sweep_update()
*******************************************************************************/
short int sweep_update(sweep* sw)
{
  if (sw == NULL)
    return 1;

  /* if the sweep is off, return */
  if (sw->mode == SWEEP_MODE_OFF)
    return 0;

  /* if the sweep has already reached the target, return */
  if (sw->start + sw->step_coarse == sw->target)
    return 0;

  /* update phase */
  sw->phase += sw->increment;

  /* check if a period was completed */

  /* note that the phase register is 28 bits (overflows once per coarse step) */
  /* we treat this as 22 bits instead (overflows once per fine step)          */
  if (sw->phase > 0x3FFFFF)
  {
    sw->phase &= 0x3FFFFF;

    /* update fine step */
    if (sw->start < sw->target)
      sw->step_fine += 1;
    else if (sw->start > sw->target)
      sw->step_fine -= 1;

    /* update coarse step */
    if ((sw->mode == SWEEP_MODE_PORTAMENTO_UP)             || 
        (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_UP)    || 
        (sw->mode == SWEEP_MODE_PORTAMENTO_DOWN)           || 
        (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_DOWN)  || 
        (sw->mode == SWEEP_MODE_PORTAMENTO_TO_NOTE)        || 
        (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_TO_NOTE))
    {
      if ((sw->start < sw->target) && (sw->step_fine >= 64))
      {
        sw->step_fine -= 64;
        sw->step_coarse += 1;
      }
      else if ((sw->start > sw->target) && (sw->step_fine <= -64))
      {
        sw->step_fine += 64;
        sw->step_coarse -= 1;
      }
    }
    else if ( (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_UP)   || 
              (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_DOWN) || 
              (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_TO_NOTE))
    {
      if ((sw->start < sw->target) && (sw->step_fine >= 128))
      {
        sw->step_fine -= 128;
        sw->step_coarse += 2;
      }
      else if ((sw->start > sw->target) && (sw->step_fine <= -128))
      {
        sw->step_fine += 128;
        sw->step_coarse -= 2;
      }
    }

    /* clamp to target */
    if ((sw->start < sw->target) && (sw->start + sw->step_coarse >= sw->target))
    {
      sw->step_coarse = sw->target - sw->start;
      sw->step_fine = 0;
    }
    else if ((sw->start > sw->target) && (sw->start + sw->step_coarse <= sw->target))
    {
      sw->step_coarse = sw->target - sw->start;
      sw->step_fine = 0;
    }

    /* set level */
    if ((sw->mode == SWEEP_MODE_PORTAMENTO_UP) || 
        (sw->mode == SWEEP_MODE_PORTAMENTO_DOWN))
    {
      sw->level = (sw->step_coarse * 64) + sw->step_fine;
    }
    else if ( (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_UP)   || 
              (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_DOWN) || 
              (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_UP)  || 
              (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_DOWN))
    {
      sw->level = sw->step_coarse * 64;
    }
    else if (sw->mode == SWEEP_MODE_PORTAMENTO_TO_NOTE)
    {
      sw->level = (sw->start - sw->target) * 64;
      sw->level += (sw->step_coarse * 64) + sw->step_fine;
    }
    else if ( (sw->mode == SWEEP_MODE_HALF_STEP_GLISSANDO_TO_NOTE) || 
              (sw->mode == SWEEP_MODE_WHOLE_STEP_GLISSANDO_TO_NOTE))
    {
      sw->level = (sw->start - sw->target) * 64;
      sw->level += sw->step_coarse * 64;
    }
  }

  return 0;
}

/*******************************************************************************
** sweep_generate_tables()
*******************************************************************************/
short int sweep_generate_tables()
{
  int m;
  int n;

  /* phase increment table */
  for (m = TEMPO_LOWER_BOUND; m <= TEMPO_UPPER_BOUND; m++)
  {
    for (n = 0; n < 9; n++)
    {
      S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][n] = 
        (int) ((TEMPO_COMPUTE_BEAT_FREQUENCY(m) * S_sweep_speed_table[n] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* testing: print tables */
#if 0
  printf("Sweep Phase Increment Table (at 120 BPM):\n");

  for (n = 0; n < 9; n++)
  {
    printf("Speed: %d, Phase Inc: %d\n", n, 
              S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(120)][n]);
  }
#endif

#if 0
  printf("Sweep Phase Increment Table (at 150 BPM):\n");

  for (n = 0; n < 9; n++)
  {
    printf("Speed: %d, Phase Inc: %d\n", n, 
              S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(150)][n]);
  }
#endif

  return 0;
}

