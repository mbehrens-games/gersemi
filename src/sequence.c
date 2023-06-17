/*******************************************************************************
** sequence.c (sequencer)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "sequence.h"
#include "synth.h"
#include "tempo.h"

/* sequencer phase increment table  */
/* 1st index: tempo                 */
/* 2nd index: swing                 */
/* 3rd index: half beat             */
static unsigned int S_sequencer_phase_increment_table[TEMPO_NUM_BPMS][TEMPO_NUM_SWINGS][2];

/* pattern array */
pattern     G_sequencer_patterns[SEQUENCER_MAX_PATTERNS];
int         G_sequencer_num_patterns;

/* time signature */
int         G_sequencer_time_signature;

/* tempo & swing */
int         G_sequencer_music_tempo;
int         G_sequencer_music_swing;

/* internal sequencer variables */
static int  S_sequencer_beats_per_bar;
static int  S_sequencer_beat_size;

static int  S_sequencer_pattern_index;
static int  S_sequencer_step_index;

static char S_sequencer_current_volume;
static char S_sequencer_current_brightness;

static unsigned int S_sequencer_phase;
static unsigned int S_sequencer_increment;

#define SEQUENCER_SET_PHASE_INCREMENT(tempo, swing, step)                                 \
  S_sequencer_increment =                                                                 \
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(tempo)][swing][(step % 4) / 2]; \
                                                                                          \
  if (S_sequencer_beat_size == 8)                                                         \
    S_sequencer_increment *= 2;                                                           \
  else if (S_sequencer_beat_size == 2)                                                    \
    S_sequencer_increment /= 2;

/*******************************************************************************
** sequencer_clear_pattern()
*******************************************************************************/
short int sequencer_clear_pattern(pattern* p)
{
  int n;

  step* s;

  /* make sure pattern is valid */
  if (p == NULL)
    return 1;

  /* clear this pattern */
  for (n = 0; n < SEQUENCER_MAX_STEPS_PER_PATTERN; n++)
  {
    s = &p->steps[n];

    s->note_1 = 0;
    s->note_2 = 0;
    s->note_3 = 0;
    s->note_4 = 0;

    s->modulation = 0;

    s->volume = 0;
    s->brightness = 0;

    s->arpeggio_mode = 0;
    s->arpeggio_speed = 0;

    s->pitch_sweep_mode = 0;
    s->pitch_sweep_speed = 0;

    s->vibrato_depth = 0;
    s->vibrato_speed = 0;

    s->tremolo_depth = 0;
    s->tremolo_speed = 0;

    s->wobble_depth = 0;
    s->wobble_speed = 0;
  }

  p->num_steps = 0;
  p->number_of_bars = 0;

  return 0;
}

/*******************************************************************************
** sequencer_setup()
*******************************************************************************/
short int sequencer_setup()
{
  int m;

  for (m = 0; m < SEQUENCER_MAX_PATTERNS; m++)
    sequencer_clear_pattern(&G_sequencer_patterns[m]);

  G_sequencer_num_patterns = 0;

  G_sequencer_time_signature = SEQUENCER_TIME_SIGNATURE_4_4;

  G_sequencer_music_tempo = 120;
  G_sequencer_music_swing = TEMPO_SWING_1_1;

  S_sequencer_beats_per_bar = 4;
  S_sequencer_beat_size = 4;

  S_sequencer_pattern_index = 0;
  S_sequencer_step_index = 0;

  S_sequencer_current_volume = 5;
  S_sequencer_current_brightness = 5;

  S_sequencer_phase = 0;

  SEQUENCER_SET_PHASE_INCREMENT(G_sequencer_music_tempo, G_sequencer_music_swing, S_sequencer_step_index)

  return 0;
}

/*******************************************************************************
** sequencer_reset()
*******************************************************************************/
short int sequencer_reset()
{
  S_sequencer_pattern_index = 0;
  S_sequencer_step_index = 0;

  return 0;
}

/*******************************************************************************
** sequencer_set_time_signature()
*******************************************************************************/
short int sequencer_set_time_signature(int sig)
{
  /* make sure time signature is valid */
  if ((sig < 0) || (sig >= SEQUENCER_NUM_TIME_SIGNATURES))
    return 1;

  /* set time signature */
  G_sequencer_time_signature = sig;

  /* set derived values */
  if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_4_4)
  {
    S_sequencer_beats_per_bar = 4;
    S_sequencer_beat_size = 4;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_3_4)
  {
    S_sequencer_beats_per_bar = 3;
    S_sequencer_beat_size = 4;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_2_4)
  {
    S_sequencer_beats_per_bar = 2;
    S_sequencer_beat_size = 4;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_2_2)
  {
    S_sequencer_beats_per_bar = 2;
    S_sequencer_beat_size = 2;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_3_8)
  {
    S_sequencer_beats_per_bar = 3;
    S_sequencer_beat_size = 8;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_6_8)
  {
    S_sequencer_beats_per_bar = 6;
    S_sequencer_beat_size = 8;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_9_8)
  {
    S_sequencer_beats_per_bar = 9;
    S_sequencer_beat_size = 8;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_12_8)
  {
    S_sequencer_beats_per_bar = 12;
    S_sequencer_beat_size = 8;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_5_4)
  {
    S_sequencer_beats_per_bar = 5;
    S_sequencer_beat_size = 4;
  }
  else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_6_4)
  {
    S_sequencer_beats_per_bar = 6;
    S_sequencer_beat_size = 4;
  }
  else
  {
    S_sequencer_beats_per_bar = 4;
    S_sequencer_beat_size = 4;
  }

  /* update phase increment */
  SEQUENCER_SET_PHASE_INCREMENT(G_sequencer_music_tempo, G_sequencer_music_swing, S_sequencer_step_index)

  return 0;
}

/*******************************************************************************
** sequencer_set_tempo()
*******************************************************************************/
short int sequencer_set_tempo(int tempo)
{
  /* make sure this tempo is valid */
  if ((tempo < TEMPO_LOWER_BOUND) || (tempo > TEMPO_UPPER_BOUND))
    return 1;

  /* set tempo */
  G_sequencer_music_tempo = tempo;

  /* update phase increment */
  SEQUENCER_SET_PHASE_INCREMENT(G_sequencer_music_tempo, G_sequencer_music_swing, S_sequencer_step_index)

  return 0;
}

/*******************************************************************************
** sequencer_set_swing()
*******************************************************************************/
short int sequencer_set_swing(int swing)
{
  /* make sure this swing is valid */
  if ((swing < 0) || (swing > TEMPO_NUM_SWINGS))
    return 1;

  /* set swing */
  G_sequencer_music_swing = swing;

  /* update phase increment */
  SEQUENCER_SET_PHASE_INCREMENT(G_sequencer_music_tempo, G_sequencer_music_swing, S_sequencer_step_index)

  return 0;
}

/*******************************************************************************
** sequencer_activate_step()
*******************************************************************************/
short int sequencer_activate_step()
{
  pattern*  p;
  step*     s;

  /* set current pattern */
  if ((S_sequencer_pattern_index >= 0) && (S_sequencer_pattern_index < G_sequencer_num_patterns))
    p = &G_sequencer_patterns[S_sequencer_pattern_index];
  else
    return 1;

  /* set current step */
  if ((S_sequencer_step_index >= 0) && (S_sequencer_step_index < p->num_steps))
    s = &p->steps[S_sequencer_step_index];
  else
    return 1;

  /* process volume & brightness columns */
  if (s->volume != 0)
    S_sequencer_current_volume = s->volume;

  if (s->brightness != 0)
    S_sequencer_current_brightness = s->brightness;

#if 0
  /* process sweep column */
  if (s->pitch_sweep_speed != 0)
  {
    synth_set_pitch_sweep(0, s->pitch_sweep_mode, G_sequencer_music_tempo, s->pitch_sweep_speed);
    synth_set_pitch_sweep(1, s->pitch_sweep_mode, G_sequencer_music_tempo, s->pitch_sweep_speed);
    synth_set_pitch_sweep(2, s->pitch_sweep_mode, G_sequencer_music_tempo, s->pitch_sweep_speed);
    synth_set_pitch_sweep(3, s->pitch_sweep_mode, G_sequencer_music_tempo, s->pitch_sweep_speed);
  }
#endif

#if 0
  /* process lfo columns */
  if (s->vibrato_speed != 0)
  {
    synth_set_vibrato(0, s->vibrato_depth, G_sequencer_music_tempo, s->vibrato_speed);
    synth_set_vibrato(1, s->vibrato_depth, G_sequencer_music_tempo, s->vibrato_speed);
    synth_set_vibrato(2, s->vibrato_depth, G_sequencer_music_tempo, s->vibrato_speed);
    synth_set_vibrato(3, s->vibrato_depth, G_sequencer_music_tempo, s->vibrato_speed);
  }

  if (s->tremolo_speed != 0)
  {
    synth_set_tremolo(0, s->tremolo_depth, G_sequencer_music_tempo, s->tremolo_speed);
    synth_set_tremolo(1, s->tremolo_depth, G_sequencer_music_tempo, s->tremolo_speed);
    synth_set_tremolo(2, s->tremolo_depth, G_sequencer_music_tempo, s->tremolo_speed);
    synth_set_tremolo(3, s->tremolo_depth, G_sequencer_music_tempo, s->tremolo_speed);
  }

  if (s->wobble_speed != 0)
  {
    synth_set_wobble(0, s->wobble_depth, G_sequencer_music_tempo, s->wobble_speed);
    synth_set_wobble(1, s->wobble_depth, G_sequencer_music_tempo, s->wobble_speed);
    synth_set_wobble(2, s->wobble_depth, G_sequencer_music_tempo, s->wobble_speed);
    synth_set_wobble(3, s->wobble_depth, G_sequencer_music_tempo, s->wobble_speed);
  }
#endif

#if 0
  /* prepare key on / key off commands */
  if (s->note_1 == 37)
    synth_key_off(0);
  else if ((s->note_1 >= 1) && (s->note_1 <= 36))
  {
    synth_key_on(0, S_key_to_note_table[S_sequencer_key_index][S_sequencer_mode_index][s->note_1 - 1], 
                    S_sequencer_current_volume, S_sequencer_current_brightness);
  }

  if (s->note_2 == 37)
    synth_key_off(1);
  else if ((s->note_2 >= 1) && (s->note_2 <= 36))
  {
    synth_key_on(1, S_key_to_note_table[S_sequencer_key_index][S_sequencer_mode_index][s->note_2 - 1], 
                    S_sequencer_current_volume, S_sequencer_current_brightness);
  }

  if (s->note_3 == 37)
    synth_key_off(2);
  else if ((s->note_3 >= 1) && (s->note_3 <= 36))
  {
    synth_key_on(2, S_key_to_note_table[S_sequencer_key_index][S_sequencer_mode_index][s->note_3 - 1], 
                    S_sequencer_current_volume, S_sequencer_current_brightness);
  }

  if (s->note_4 == 37)
    synth_key_off(3);
  else if ((s->note_4 >= 1) && (s->note_4 <= 36))
  {
    synth_key_on(3, S_key_to_note_table[S_sequencer_key_index][S_sequencer_mode_index][s->note_4 - 1], 
                    S_sequencer_current_volume, S_sequencer_current_brightness);
  }
#endif

  return 0;
}

/*******************************************************************************
** sequencer_update()
*******************************************************************************/
short int sequencer_update()
{
  pattern* p;

  /* if all patterns played, return */
  if ((S_sequencer_pattern_index < 0) || (S_sequencer_pattern_index >= G_sequencer_num_patterns))
    return 0;

  /* update phase */
  S_sequencer_phase += S_sequencer_increment;

  /* check if a period was completed */
  if (S_sequencer_phase > 0xFFFFFFF)
  {
#if 0
    S_sequencer_phase &= 0xFFFFFFF;
#else
    S_sequencer_phase = 0;
#endif

    /* set current pattern */
    p = &G_sequencer_patterns[S_sequencer_pattern_index];

    /* increment current step */
    S_sequencer_step_index += 1;

    /* see if this pattern is completed */
    if (S_sequencer_step_index >= p->num_steps)
    {
      S_sequencer_pattern_index += 1;

      if ((S_sequencer_pattern_index < 0) || (S_sequencer_pattern_index >= G_sequencer_num_patterns))
        return 0;

      p = &G_sequencer_patterns[S_sequencer_pattern_index];

      S_sequencer_step_index = 0;
    }

    /* process this step */
    sequencer_activate_step();

    /* update the phase increment */
    SEQUENCER_SET_PHASE_INCREMENT(G_sequencer_music_tempo, G_sequencer_music_swing, S_sequencer_step_index)
  }

  return 0;
}

/*******************************************************************************
** sequencer_generate_tables()
*******************************************************************************/
short int sequencer_generate_tables()
{
  int m;

  /* sequencer phase increment table */
  for (m = TEMPO_LOWER_BOUND; m <= TEMPO_UPPER_BOUND; m++)
  {
    /* at beat size 4, each step is a 1/16 note (1/4 of a quarter note),    */
    /* so the step frequency is 4 times the beat (quarter note) frequency.  */

    /* 1:1 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_1_1][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 1, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_1_1][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 1, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 5:4 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_4][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 5, 4) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_4][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 5, 4) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 4:3 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_4_3][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 4, 3) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_4_3][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 4, 3) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 3:2 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_3_2][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 3, 2) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_3_2][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 3, 2) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 5:3 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_3][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 5, 3) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_3][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 5, 3) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 2:1 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_2_1][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 2, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_2_1][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 2, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 5:2 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_2][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 5, 2) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_5_2][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 5, 2) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    /* 3:1 swing */
    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_3_1][0] = 
      (int) ((TEMPO_COMPUTE_FIRST_HALF_BEAT_FREQUENCY(m, 3, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);

    S_sequencer_phase_increment_table[TEMPO_COMPUTE_INDEX(m)][TEMPO_SWING_3_1][1] = 
      (int) ((TEMPO_COMPUTE_SECOND_HALF_BEAT_FREQUENCY(m, 3, 1) * 4.0f * CLOCK_1HZ_PHASE_INCREMENT) + 0.5f);
  }

  return 0;
}
