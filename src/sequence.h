/*******************************************************************************
** sequence.h (sequencer)
*******************************************************************************/

#ifndef SEQUENCE_H
#define SEQUENCE_H

enum
{
  SEQUENCER_TIME_SIGNATURE_4_4 = 0,
  SEQUENCER_TIME_SIGNATURE_3_4,
  SEQUENCER_TIME_SIGNATURE_2_4,
  SEQUENCER_TIME_SIGNATURE_2_2,
  SEQUENCER_TIME_SIGNATURE_3_8,
  SEQUENCER_TIME_SIGNATURE_6_8,
  SEQUENCER_TIME_SIGNATURE_9_8,
  SEQUENCER_TIME_SIGNATURE_12_8,
  SEQUENCER_TIME_SIGNATURE_5_4,
  SEQUENCER_TIME_SIGNATURE_6_4,
  SEQUENCER_NUM_TIME_SIGNATURES
};

#define SEQUENCER_MAX_PATTERNS            16
#define SEQUENCER_MAX_STEPS_PER_PATTERN   128 /* 4 steps per beat, 4 beats per bar, 8 bars in a pattern */

typedef struct step
{
  char note_1;
  char note_2;
  char note_3;
  char note_4;

  char modulation;

  char volume;
  char brightness;

  char arpeggio_mode;
  char arpeggio_speed;

  char pitch_sweep_mode;
  char pitch_sweep_speed;

  char vibrato_depth;
  char vibrato_speed;

  char tremolo_depth;
  char tremolo_speed;

  char wobble_depth;
  char wobble_speed;
} step;

typedef struct pattern
{
  step  steps[SEQUENCER_MAX_STEPS_PER_PATTERN];
  int   num_steps;

  int   number_of_bars;
} pattern;

extern pattern  G_sequencer_patterns[SEQUENCER_MAX_PATTERNS];
extern int      G_sequencer_num_patterns;

extern int      G_sequencer_time_signature;

extern int      G_sequencer_music_tempo;
extern int      G_sequencer_music_swing;

/* function declarations */
short int   sequencer_setup();

short int   sequencer_reset();

short int   sequencer_set_time_signature(int sig);

short int   sequencer_set_tempo(int tempo);
short int   sequencer_set_swing(int swing);

short int   sequencer_activate_step();

short int   sequencer_update();

short int   sequencer_generate_tables();

#endif
