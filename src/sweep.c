/*******************************************************************************
** sweep.c (portamento / glissando)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "midicont.h"
#include "patch.h"
#include "sweep.h"
#include "tempo.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

/* sweep speed table */

/* frequency table (in semitones per beat) */
static float S_sweep_frequency_table[PATCH_PORTAMENTO_SPEED_NUM_VALUES] = 
              { 1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f, 
                9.0f, 10.0f, 11.0f, 12.0f, 16.0f, 19.0f, 22.0f, 24.0f 
              };

/* phase increment table */
static unsigned int S_sweep_phase_increment_table[TEMPO_NUM_VALUES][PATCH_PORTAMENTO_SPEED_NUM_VALUES];

/* sweep bank */
sweep G_sweep_bank[BANK_NUM_SWEEPS];

/*******************************************************************************
** sweep_reset_all()
*******************************************************************************/
short int sweep_reset_all()
{
  int k;

  sweep* sw;

  /* setup all sweeps */
  for (k = 0; k < BANK_NUM_SWEEPS; k++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[k];

    /* initialize sweep variables */
    sw->mode = PATCH_PORTAMENTO_MODE_DEFAULT;
    sw->speed = PATCH_PORTAMENTO_SPEED_DEFAULT;

    sw->phase = 0;

    sw->increment = 
      S_sweep_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_PORTAMENTO_SPEED_DEFAULT - PATCH_PORTAMENTO_SPEED_LOWER_BOUND];

    sw->note = TUNING_NOTE_BLANK;
    sw->offset = 0;

    sw->tempo = TEMPO_DEFAULT;

    sw->level = 0;
  }

  return 0;
}

/*******************************************************************************
** sweep_load_patch()
*******************************************************************************/
short int sweep_load_patch(int instrument_index, int patch_index)
{
  sweep* sw;
  patch* p;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* mode */
  if ((p->portamento_mode >= PATCH_PORTAMENTO_MODE_LOWER_BOUND) && 
      (p->portamento_mode <= PATCH_PORTAMENTO_MODE_UPPER_BOUND))
  {
    sw->mode = p->portamento_mode;
  }
  else
    sw->mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND;

  /* speed */
  if ((p->portamento_speed >= PATCH_PORTAMENTO_SPEED_LOWER_BOUND) && 
      (p->portamento_speed <= PATCH_PORTAMENTO_SPEED_UPPER_BOUND))
  {
    sw->speed = p->portamento_speed;
    sw->increment = S_sweep_phase_increment_table[sw->tempo - TEMPO_LOWER_BOUND][p->portamento_speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND];
  }
  else
  {
    sw->speed = PATCH_PORTAMENTO_SPEED_LOWER_BOUND;
    sw->increment = S_sweep_phase_increment_table[sw->tempo - TEMPO_LOWER_BOUND][0];
  }

  return 0;
}

/*******************************************************************************
** sweep_set_tempo()
*******************************************************************************/
short int sweep_set_tempo(int instrument_index, short int tempo)
{
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* set tempo */
  if (tempo < TEMPO_LOWER_BOUND)
    sw->tempo = TEMPO_LOWER_BOUND;
  else if (tempo > TEMPO_UPPER_BOUND)
    sw->tempo = TEMPO_UPPER_BOUND;
  else
    sw->tempo = tempo;

  /* adjust phase increment based on tempo */
  sw->increment = 
    S_sweep_phase_increment_table[sw->tempo - TEMPO_LOWER_BOUND][sw->speed - PATCH_PORTAMENTO_SPEED_LOWER_BOUND];

  return 0;
}

/*******************************************************************************
** sweep_set_note()
*******************************************************************************/
short int sweep_set_note(int instrument_index, int note)
{
  sweep* sw;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* determine if the new note is valid */
  if (note == TUNING_NOTE_BLANK)
    return 0;

  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* set the note */
  sw->note = note;

  return 0;
}

/*******************************************************************************
** sweep_trigger()
*******************************************************************************/
short int sweep_trigger(int instrument_index, int note)
{
  sweep* sw;

  int tuning_index;

  /* make sure that the instrument index is valid */
  if (BANK_INSTRUMENT_INDEX_IS_NOT_VALID(instrument_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[instrument_index];

  /* determine if the new note is valid */
  if (note == TUNING_NOTE_BLANK)
    return 0;

  if ((note < TUNING_NOTE_A0) || (note > TUNING_NOTE_C8))
    return 0;

  /* if the starting note is undefined, just set the note and return */
  if (sw->note == TUNING_NOTE_BLANK)
  {
    sw->note = note;
    sw->offset = 0;

    return 0;
  }

  /* calculate the offset based on the current  */
  /* position of the sweep in the tuning table  */
  tuning_index = (sw->note * TUNING_NUM_SEMITONE_STEPS) + sw->offset;

  sw->note = note;
  sw->offset = tuning_index - (note * TUNING_NUM_SEMITONE_STEPS);

  /* reset phase */
  sw->phase = 0;

  /* set level based on mode */
  if (sw->mode == PATCH_PORTAMENTO_MODE_BEND)
    sw->level = sw->offset;
  else if (sw->mode == PATCH_PORTAMENTO_MODE_SEMITONES)
  {
    if (sw->offset >= 0)
    {
      sw->level = sw->offset;
      sw->level /= TUNING_NUM_SEMITONE_STEPS;
      sw->level *= TUNING_NUM_SEMITONE_STEPS;
    }
    else
    {
      sw->level = -sw->offset;
      sw->level /= TUNING_NUM_SEMITONE_STEPS;
      sw->level *= -TUNING_NUM_SEMITONE_STEPS;
    }
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
  for (k = 0; k < BANK_NUM_SWEEPS; k++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[k];

    /* update phase */
    sw->phase += sw->increment;

    /* check if a period was completed */

    /* wraparound phase register (28 bits) */
    if (sw->phase > 0xFFFFFFF)
    {
      if (sw->offset < 0)
        sw->offset += 1;
      else if (sw->offset > 0)
        sw->offset -= 1;

      sw->phase &= 0xFFFFFFF;
    }

    /* set level based on mode */
    if (sw->mode == PATCH_PORTAMENTO_MODE_BEND)
      sw->level = sw->offset;
    else if (sw->mode == PATCH_PORTAMENTO_MODE_SEMITONES)
    {
      if (sw->offset >= 0)
      {
        sw->level = sw->offset;
        sw->level /= TUNING_NUM_SEMITONE_STEPS;
        sw->level *= TUNING_NUM_SEMITONE_STEPS;
      }
      else
      {
        sw->level = -sw->offset;
        sw->level /= TUNING_NUM_SEMITONE_STEPS;
        sw->level *= -TUNING_NUM_SEMITONE_STEPS;
      }
    }
    else
      sw->level = 0;
  }

  return 0;
}

/*******************************************************************************
** sweep_generate_tables()
*******************************************************************************/
short int sweep_generate_tables()
{
  int k;
  int m;

  /* phase increment table */
  for (k = 0; k < TEMPO_NUM_VALUES; k++)
  {
    for (m = 0; m < PATCH_PORTAMENTO_SPEED_NUM_VALUES; m++)
    {
      S_sweep_phase_increment_table[k][m] = 
        (int) ((TEMPO_COMPUTE_BEATS_PER_SECOND(k + TEMPO_LOWER_BOUND) * S_sweep_frequency_table[m] * TUNING_NUM_SEMITONE_STEPS * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
    }
  }

  return 0;
}

