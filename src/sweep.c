/*******************************************************************************
** sweep.c (pitch sweep, duty cycle sweep)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "sweep.h"
#include "tempo.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define SWEEP_LOWER_BOUND (0 * 12 + 9) /* A0 */
#define SWEEP_UPPER_BOUND (8 * 12 + 0) /* C8 */

#define SWEEP_NUM_SPEEDS  16

/* sweep phase increment table  */
/* 1st index: tempo             */
/* 2nd index: sweep speed       */
static unsigned int S_sweep_phase_increment_table[TEMPO_NUM_BPMS][SWEEP_NUM_SPEEDS];

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
static float S_sweep_speed_table[SWEEP_NUM_SPEEDS] = 
              { 0.5f, 
                1.0f, 
                1.5f, 
                2.0f, 
                2.5f, 
                3.0f, 
                3.5f, 
                4.0f, 
                5.0f, 
                6.0f, 
                7.0f, 
                8.0f, 
                9.0f, 
               10.0f, 
               11.0f, 
               12.0f 
              };


/* sweep bank */
sweep G_sweep_bank[BANK_NUM_SWEEPS];

/*******************************************************************************
** sweep_setup_all()
*******************************************************************************/
short int sweep_setup_all()
{
  int k;

  /* setup all sweeps */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    sweep_reset(k);

  return 0;
}

/*******************************************************************************
** sweep_reset()
*******************************************************************************/
short int sweep_reset(int voice_index)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

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
short int sweep_set_mode(int voice_index, int mode)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* set mode */
  if ((mode >= 0) && (mode < SWEEP_NUM_MODES))
    sw->mode = mode;
  else
    sw->mode = 0;

  return 0;
}

/*******************************************************************************
** sweep_set_speed()
*******************************************************************************/
short int sweep_set_speed(int voice_index, int tempo, int speed)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* make sure tempo is valid */
  if (TEMPO_IS_NOT_VALID(tempo))
    return 0;

  /* set the speed */
  if ((speed >= 1) && (speed <= SWEEP_NUM_SPEEDS))
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
short int sweep_adjust_to_tempo(int voice_index, int tempo)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

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
short int sweep_trigger(int voice_index, int old_note, int new_note)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

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
** sweep_update_all()
*******************************************************************************/
short int sweep_update_all()
{
  int k;

  sweep* sw;

  /* update all sweeps */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    sw = &G_sweep_bank[k];

    /* if the sweep is off, continue */
    if (sw->mode == SWEEP_MODE_OFF)
      continue;

    /* if the sweep has already reached the target, continue */
    if (sw->start + sw->step_coarse == sw->target)
      continue;

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
    for (n = 0; n < SWEEP_NUM_SPEEDS; n++)
    {
      S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][n] = 
        (int) ((TEMPO_COMPUTE_BEAT_FREQUENCY(m) * S_sweep_speed_table[n] * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  /* testing: print tables */
#if 0
  printf("Sweep Phase Increment Table (at 120 BPM):\n");

  for (n = 0; n < SWEEP_NUM_SPEEDS; n++)
  {
    printf("Speed: %d, Phase Inc: %d\n", n, 
              S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(120)][n]);
  }
#endif

#if 0
  printf("Sweep Phase Increment Table (at 150 BPM):\n");

  for (n = 0; n < SWEEP_NUM_SPEEDS; n++)
  {
    printf("Speed: %d, Phase Inc: %d\n", n, 
              S_sweep_phase_increment_table[TEMPO_COMPUTE_INDEX(150)][n]);
  }
#endif

  return 0;
}

