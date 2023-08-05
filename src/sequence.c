/*******************************************************************************
** sequence.c (sequencer)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "sequence.h"
#include "synth.h"
#include "tempo.h"

/* pattern bank */
pattern G_sequencer_pattern_bank[BANK_NUM_PATTERNS];

/* tempo phase increment table  */
static unsigned int S_sequencer_tempo_phase_increment_table[TEMPO_NUM_BPMS];

/* swing intervals phase table */
static unsigned int S_sequencer_swing_theta_table[TEMPO_NUM_SWINGS][SEQUENCER_NUM_STEPS_PER_BEAT - 1];

/* the swing ratios are kept in the approx. range of 1:1 to 3:1 (0.5 to 0.75) */
static float S_sequencer_swing_multiplier_table[TEMPO_NUM_SWINGS] = 
              { 1.0f,                                 /* 1:1 -> 1/2 = 0.5   */
                TEMPO_COMPUTE_SWING_MULTIPLIER(5, 4), /* 5:4 -> 5/9 = 0.556 */
                TEMPO_COMPUTE_SWING_MULTIPLIER(4, 3), /* 4:3 -> 4/7 = 0.571 */
                TEMPO_COMPUTE_SWING_MULTIPLIER(3, 2), /* 3:2 -> 3/5 = 0.6   */
                TEMPO_COMPUTE_SWING_MULTIPLIER(5, 3), /* 5:3 -> 5/8 = 0.625 */
                TEMPO_COMPUTE_SWING_MULTIPLIER(2, 1), /* 2:1 -> 2/3 = 0.667 */
                TEMPO_COMPUTE_SWING_MULTIPLIER(5, 2), /* 5:2 -> 5/7 = 0.714 */
                TEMPO_COMPUTE_SWING_MULTIPLIER(3, 1)  /* 3:1 -> 3/4 = 0.75  */
              };

/* internal sequencer variables */
static int  S_sequencer_music_beats_per_bar;
static int  S_sequencer_music_beat_size;

static int  S_sequencer_music_pattern_index;
static int  S_sequencer_music_beat_index;
static int  S_sequencer_music_step_index;

static unsigned int S_sequencer_music_phase;
static unsigned int S_sequencer_music_increment;

static instrument_step S_sequencer_music_current_instrument_settings[SEQUENCER_NUM_INSTRUMENTS];

#define SEQUENCER_SET_MUSIC_TEMPO_PHASE_INCREMENT(tempo)                       \
  S_sequencer_music_increment =                                                \
    S_sequencer_tempo_phase_increment_table[tempo - TEMPO_BPM_LOWER_BOUND];    \
                                                                               \
  if (S_sequencer_music_beat_size == 8)                                        \
    S_sequencer_music_increment *= 2;                                          \
  else if (S_sequencer_music_beat_size == 2)                                   \
    S_sequencer_music_increment /= 2;

/*******************************************************************************
** sequencer_pattern_setup_all()
*******************************************************************************/
short int sequencer_pattern_setup_all()
{
  int k;

  /* setup all patterns */
  for (k = 0; k < BANK_NUM_PATTERNS; k++)
    sequencer_pattern_reset(k);

  return 0;
}

/*******************************************************************************
** sequencer_pattern_reset()
*******************************************************************************/
short int sequencer_pattern_reset(int pattern_index)
{
  int m;
  int n;

  pattern* p;

  drum_step*        dr_st;
  instrument_step*  in_st;

  /* make sure that the pattern index is valid */
  if (BANK_PATTERN_INDEX_IS_NOT_VALID(pattern_index))
    return 1;

  /* obtain pattern pointer */
  p = &G_sequencer_pattern_bank[pattern_index];

  /* reset tempos */
  for (n = 0; n < SEQUENCER_NUM_BEATS_PER_PATTERN; n++)
    p->tempo[n] = 120;

  /* reset swings */
  for (m = 0; m < SEQUENCER_NUM_MUSIC_SWINGS; m++)
  {
    for (n = 0; n < SEQUENCER_NUM_BEATS_PER_PATTERN; n++)
      p->swings[m][n] = TEMPO_SWING_LOWER_BOUND;
  }

  /* reset drum steps */
  for (m = 0; m < SEQUENCER_NUM_DRUM_VOICES; m++)
  {
    for (n = 0; n < SEQUENCER_NUM_STEPS_PER_PATTERN; n++)
    {
      dr_st = &p->dr_steps[m][n];

      dr_st->note[0] = 0;
      dr_st->note[1] = 0;
      dr_st->note[2] = 0;
      dr_st->note[3] = 0;
      dr_st->note[4] = 0;
      dr_st->note[5] = 0;
    }
  }

  /* reset instrument steps */
  for (m = 0; m < SEQUENCER_NUM_INSTRUMENTS; m++)
  {
    for (n = 0; n < SEQUENCER_NUM_STEPS_PER_PATTERN; n++)
    {
      in_st = &p->in_steps[m][n];

      in_st->note[0] = 0;
      in_st->note[1] = 0;
      in_st->note[2] = 0;
      in_st->note[3] = 0;

      in_st->modulation = 0;

      in_st->arpeggio_mode = 0;
      in_st->arpeggio_speed = 0;

      in_st->volume = 0;

      in_st->portamento_switch = 0;
      in_st->portamento_speed = 0;

      in_st->mod_wheel_amount = 0;
      in_st->aftertouch_amount = 0;
    }
  }

  p->num_bars = 0;

  return 0;
}

/*******************************************************************************
** sequencer_setup()
*******************************************************************************/
short int sequencer_setup()
{
  sequencer_pattern_setup_all();

  S_sequencer_music_beats_per_bar = 4;
  S_sequencer_music_beat_size = 4;

  S_sequencer_music_pattern_index = 0;
  S_sequencer_music_beat_index = 0;
  S_sequencer_music_step_index = 0;

  S_sequencer_music_phase = 0;
  S_sequencer_music_increment = 0;

  return 0;
}

/*******************************************************************************
** sequencer_reset()
*******************************************************************************/
short int sequencer_reset()
{
  S_sequencer_music_pattern_index = 0;
  S_sequencer_music_beat_index = 0;
  S_sequencer_music_step_index = 0;

  S_sequencer_music_phase = 0;
  S_sequencer_music_increment = 0;

  return 0;
}

/*******************************************************************************
** sequencer_activate_step()
*******************************************************************************/
short int sequencer_activate_step()
{
  pattern* p;

  instrument_step* in_st;

  /* set current pattern */
  if (BANK_PATTERN_INDEX_IS_NOT_VALID(S_sequencer_music_pattern_index))
    return 1;

  p = &G_sequencer_pattern_bank[S_sequencer_music_pattern_index];

  /* set current step */
  if ((S_sequencer_music_beat_index >= 0) && (S_sequencer_music_beat_index < SEQUENCER_NUM_BEATS_PER_PATTERN) && 
      (S_sequencer_music_step_index >= 0) && (S_sequencer_music_step_index < SEQUENCER_NUM_STEPS_PER_BEAT))
  {
    in_st = &p->in_steps[0][S_sequencer_music_beat_index * SEQUENCER_NUM_STEPS_PER_BEAT + S_sequencer_music_step_index];
  }
  else
    return 1;

  /* process key on / key off commands */

  /* process mod wheel / aftertouch settings */

  return 0;
}

/*******************************************************************************
** sequencer_update()
*******************************************************************************/
short int sequencer_update()
{
  /* if all patterns played, return */
  if ((S_sequencer_music_pattern_index < 0) || (S_sequencer_music_pattern_index >= BANK_NUM_PATTERNS))
    return 0;

  /* update phase */
  S_sequencer_music_phase += S_sequencer_music_increment;

  /* check if a beat was completed */
  if (S_sequencer_music_phase > 0xFFFFFFF)
  {
    S_sequencer_music_phase &= 0xFFFFFFF;

    S_sequencer_music_beat_index += 1;
    S_sequencer_music_step_index = 0;

    /* update tempo and swing if necessary... */

    sequencer_activate_step();
  }

  /* check if a step was completed */
  if (((S_sequencer_music_step_index == 0) && (S_sequencer_music_phase > S_sequencer_swing_theta_table[0][0])) || 
      ((S_sequencer_music_step_index == 1) && (S_sequencer_music_phase > S_sequencer_swing_theta_table[0][1])) || 
      ((S_sequencer_music_step_index == 2) && (S_sequencer_music_phase > S_sequencer_swing_theta_table[0][2])))
  {
    S_sequencer_music_step_index += 1;

    sequencer_activate_step();
  }

  return 0;
}

/*******************************************************************************
** sequencer_generate_tables()
*******************************************************************************/
short int sequencer_generate_tables()
{
  int m;

  /* tempo phase increment table */
  for (m = TEMPO_BPM_LOWER_BOUND; m <= TEMPO_BPM_UPPER_BOUND; m++)
  {
    S_sequencer_tempo_phase_increment_table[m - TEMPO_BPM_LOWER_BOUND] = 
      (int) ((TEMPO_COMPUTE_BEAT_FREQUENCY(m) * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  /* swing intervals phase table */
  for (m = 0; m < TEMPO_NUM_SWINGS; m++)
  {
    /* determine middle of beat */
    S_sequencer_swing_theta_table[m][1] = 
      (int) ((0x8000000 * S_sequencer_swing_multiplier_table[m]) + 0.5f);

    /* determine middle of 1st half beat */
    S_sequencer_swing_theta_table[m][0] = 
      S_sequencer_swing_theta_table[m][1] / 2;

    /* determine middle of 2nd half beat */
    S_sequencer_swing_theta_table[m][2] = 
      0x8000000 + (S_sequencer_swing_theta_table[m][1] / 2);
  }

  return 0;
}
