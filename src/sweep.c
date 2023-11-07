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

  /* initialize sweep variables */
  sw->mode = PATCH_PORTAMENTO_MODE_DEFAULT;
  sw->legato = PATCH_PORTAMENTO_LEGATO_DEFAULT;
  sw->speed = PATCH_PORTAMENTO_SPEED_DEFAULT;

  sw->phase = 0;
  sw->increment = 
    S_sweep_phase_increment_table[TEMPO_DEFAULT - TEMPO_LOWER_BOUND][PATCH_PORTAMENTO_SPEED_DEFAULT - PATCH_PORTAMENTO_SPEED_LOWER_BOUND];;

  sw->portamento_switch = MIDI_CONT_PORTAMENTO_SWITCH_OFF;

  sw->start_note = TUNING_NOTE_BLANK;
  sw->end_note = TUNING_NOTE_BLANK;

  sw->offset = 0;

  sw->tempo = TEMPO_DEFAULT;

  sw->level = 0;

  return 0;
}

/*******************************************************************************
** sweep_load_patch()
*******************************************************************************/
short int sweep_load_patch(int voice_index, int patch_index)
{
  sweep* sw;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain patch pointer */
  p = &G_patch_bank[patch_index];

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* mode */
  if ((p->portamento_mode >= PATCH_PORTAMENTO_MODE_LOWER_BOUND) && 
      (p->portamento_mode <= PATCH_PORTAMENTO_MODE_UPPER_BOUND))
  {
    sw->mode = p->portamento_mode;
  }
  else
    sw->mode = PATCH_PORTAMENTO_MODE_LOWER_BOUND;

  /* legato */
  if ((p->portamento_legato >= PATCH_PORTAMENTO_LEGATO_LOWER_BOUND) && 
      (p->portamento_legato <= PATCH_PORTAMENTO_LEGATO_UPPER_BOUND))
  {
    sw->legato = p->portamento_legato;
  }
  else
    sw->legato = PATCH_PORTAMENTO_LEGATO_LOWER_BOUND;

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
short int sweep_set_tempo(int voice_index, short int tempo)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

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
** sweep_set_portamento_switch_off()
*******************************************************************************/
short int sweep_set_portamento_switch_off(int voice_index)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* turn the portamento switch off */
  sw->portamento_switch = MIDI_CONT_PORTAMENTO_SWITCH_OFF;

  return 0;
}

/*******************************************************************************
** sweep_set_portamento_switch_on()
*******************************************************************************/
short int sweep_set_portamento_switch_on(int voice_index)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* turn the portamento switch on */
  sw->portamento_switch = MIDI_CONT_PORTAMENTO_SWITCH_ON;

  return 0;
}

/*******************************************************************************
** sweep_trigger()
*******************************************************************************/
short int sweep_trigger(int voice_index, int new_start_note, int new_end_note)
{
  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain sweep pointer */
  sw = &G_sweep_bank[voice_index];

  /* determine if the new notes are valid */
  if ((new_start_note < TUNING_NOTE_A0) || (new_start_note > TUNING_NOTE_C8))
    return 0;

  if ((new_end_note < TUNING_NOTE_A0) || (new_end_note > TUNING_NOTE_C8))
    return 0;

  /* check if the portamento is on */
  if (sw->portamento_switch == MIDI_CONT_PORTAMENTO_SWITCH_OFF)
    return 0;

  /* if there is currently a sweep active, calculate the      */
  /* offset based on the current position on the tuning table */
  if (sw->offset != 0)
    sw->offset = sw->offset + ((sw->end_note - new_end_note) * TUNING_NUM_SEMITONE_STEPS);
  else
    sw->offset = (new_start_note - new_end_note) * TUNING_NUM_SEMITONE_STEPS;

  /* set the start and end notes in the sweep */
  sw->start_note = new_start_note;
  sw->end_note = new_end_note;

  /* reset phase */
  sw->phase = 0;

  /* set level based on mode */
  if (sw->mode == PATCH_PORTAMENTO_MODE_BEND)
    sw->level = sw->offset;
  else if (sw->mode == PATCH_PORTAMENTO_MODE_HALF_STEPS)
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
  for (k = 0; k < BANK_NUM_VOICES; k++)
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
    else if (sw->mode == PATCH_PORTAMENTO_MODE_HALF_STEPS)
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

