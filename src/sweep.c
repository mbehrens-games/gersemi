/*******************************************************************************
** sweep.c (portamento / glissando)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "patch.h"
#include "sweep.h"
#include "tuning.h"

#define PI      3.14159265358979323846f
#define TWO_PI  6.28318530717958647693f

#define SWEEP_NUM_SPEEDS  16

/* sweep speed table */

/* note that the speeds are based on a tempo of 120 bpm */
static float S_sweep_speed_table[SWEEP_NUM_SPEEDS] = 
              { 2.0f, /*   1 semitone per beat  */
                4.0f, /*   2 semitones per beat */
                6.0f, /*   3 semitones per beat */
                8.0f, /*   4 semitones per beat */
               10.0f, /*   5 semitones per beat */
               12.0f, /*   6 semitones per beat */
               14.0f, /*   7 semitones per beat */
               16.0f, /*   8 semitones per beat */
               18.0f, /*   9 semitones per beat */
               20.0f, /*  10 semitones per beat */
               22.0f, /*  11 semitones per beat */
               24.0f, /*  12 semitones per beat */
               26.0f, /*  13 semitones per beat */
               28.0f, /*  14 semitones per beat */
               30.0f, /*  15 semitones per beat */
               32.0f  /*  16 semitones per beat */
              };

/* phase increment table */
static unsigned int S_sweep_phase_increment_table[SWEEP_NUM_SPEEDS];

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
  int m;

  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_SWEEPS_PER_VOICE; m++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[BANK_SWEEPS_PER_VOICE * voice_index + m];

    /* initialize sweep variables */
    sw->port_arp_mode = 0;
    sw->port_arp_direction = 0;
    sw->port_arp_speed = 1;

    sw->phase = 0;
    sw->increment = 0;

    sw->note = 0;
    sw->offset = 0;

    sw->note_input = 0;

    sw->switch_input = 0;

    sw->level = 0;
  }

  return 0;
}

/*******************************************************************************
** sweep_load_patch()
*******************************************************************************/
short int sweep_load_patch(int voice_index, int patch_index)
{
  int m;

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

  for (m = 0; m < BANK_SWEEPS_PER_VOICE; m++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[BANK_SWEEPS_PER_VOICE * voice_index + m];

    /* portamento mode */
    if ((p->port_arp_mode >= PATCH_PORT_ARP_MODE_LOWER_BOUND) && 
        (p->port_arp_mode <= PATCH_PORT_ARP_MODE_UPPER_BOUND))
    {
      sw->port_arp_mode = p->port_arp_mode;
    }
    else
      sw->port_arp_mode = PATCH_PORT_ARP_MODE_LOWER_BOUND;

    /* portamento speed */
    if ((p->port_arp_speed >= PATCH_PORT_ARP_SPEED_LOWER_BOUND) && 
        (p->port_arp_speed <= PATCH_PORT_ARP_SPEED_UPPER_BOUND))
    {
      sw->port_arp_speed = p->port_arp_speed;
      sw->increment = S_sweep_phase_increment_table[p->port_arp_speed - PATCH_PORT_ARP_SPEED_LOWER_BOUND];
    }
    else
    {
      sw->port_arp_speed = PATCH_PORT_ARP_SPEED_LOWER_BOUND;
      sw->increment = S_sweep_phase_increment_table[0];
    }
  }

  return 0;
}

/*******************************************************************************
** sweep_trigger()
*******************************************************************************/
short int sweep_trigger(int voice_index)
{
  int m;

  sweep* sw;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  for (m = 0; m < BANK_SWEEPS_PER_VOICE; m++)
  {
    /* obtain sweep pointer */
    sw = &G_sweep_bank[BANK_SWEEPS_PER_VOICE * voice_index + m];

    /* determine if the new note is valid */
    if ((sw->note_input < TUNING_NOTE_A0) || (sw->note_input > TUNING_NOTE_C8))
      return 1;

    /* check if the portamento is on */
    if (sw->switch_input == 0)
    {
      sw->offset = 0;
      sw->note = sw->note_input;
    }
    /* if the old note was 0 (default), we don't have any portamento! */
    else if ((sw->note < TUNING_NOTE_A0) || (sw->note > TUNING_NOTE_C8))
    {
      sw->offset = 0;
      sw->note = sw->note_input;
    }
    /* otherwise, determine the starting offset */
    else
    {
      sw->offset = (sw->note - sw->note_input) * TUNING_NUM_SEMITONE_STEPS - sw->offset;
      sw->note = sw->note_input;
    }

    /* reset phase */
    sw->phase = 0;

    /* set level */
    if (sw->port_arp_mode == 0)
      sw->level = sw->offset;
    else if (sw->port_arp_mode == 1)
    {
      if (sw->offset >= 0)
        sw->level = (sw->offset / TUNING_NUM_SEMITONE_STEPS) * TUNING_NUM_SEMITONE_STEPS;
      else
        sw->level = -((-sw->offset / TUNING_NUM_SEMITONE_STEPS) * TUNING_NUM_SEMITONE_STEPS);
    }
  }

  return 0;
}

/*******************************************************************************
** sweep_update_all()
*******************************************************************************/
short int sweep_update_all()
{
  int k;
  int m;

  sweep* sw;

  /* update all sweeps */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    for (m = 0; m < BANK_SWEEPS_PER_VOICE; m++)
    {
      /* obtain sweep pointer */
      sw = &G_sweep_bank[BANK_SWEEPS_PER_VOICE * k + m];

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

      /* set level */

      /* mode 0: portamento (continuous) */
      if (sw->port_arp_mode == 0)
      {
        sw->level = sw->offset;
      }
      /* mode 1: glissando (chromatic) */
      else if (sw->port_arp_mode == 1)
      {
        if (sw->offset >= 0)
          sw->level = (sw->offset / TUNING_NUM_SEMITONE_STEPS) * TUNING_NUM_SEMITONE_STEPS;
        else
          sw->level = -((-sw->offset / TUNING_NUM_SEMITONE_STEPS) * TUNING_NUM_SEMITONE_STEPS);
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
  int i;

  /* phase increment table */
  for (i = 0; i < SWEEP_NUM_SPEEDS; i++)
  {
    S_sweep_phase_increment_table[i] = 
      (int) ((S_sweep_speed_table[i] * TUNING_NUM_SEMITONE_STEPS * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}

