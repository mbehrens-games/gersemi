/*******************************************************************************
** sequence.h (sequencer)
*******************************************************************************/

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "bank.h"

#if 0
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
#endif

#define SEQUENCER_NUM_STEPS_PER_BEAT    4
#define SEQUENCER_NUM_BEATS_PER_PATTERN 32  /* 4 beats per bar, 8 bars in a pattern */

#define SEQUENCER_NUM_STEPS_PER_PATTERN (SEQUENCER_NUM_STEPS_PER_BEAT * SEQUENCER_NUM_BEATS_PER_PATTERN)

#define SEQUENCER_NUM_SFX_VOICES  6
#define SEQUENCER_NUM_DRUM_VOICES 6
#define SEQUENCER_NUM_INSTRUMENTS 9 /* 1 poly instrument + 8 mono instruments */

#define SEQUENCER_NUM_SFX_TEMPOS  SEQUENCER_NUM_SFX_VOICES
#define SEQUENCER_NUM_SFX_SWINGS  SEQUENCER_NUM_SFX_VOICES

#define SEQUENCER_NUM_MUSIC_TEMPOS  1
#define SEQUENCER_NUM_MUSIC_SWINGS  (SEQUENCER_NUM_DRUM_VOICES + SEQUENCER_NUM_INSTRUMENTS)

typedef struct drum_step
{
  char note[6];
} drum_step;

typedef struct instrument_step
{
  char note[4];
  char modulation;

  char arpeggio_mode;
  char arpeggio_speed;

  char volume;

  char portamento_switch;
  char portamento_speed;

  char mod_wheel_amount;
  char aftertouch_amount;
} instrument_step;

typedef struct pattern
{
  char tempo[SEQUENCER_NUM_BEATS_PER_PATTERN];
  char swings[SEQUENCER_NUM_MUSIC_SWINGS][SEQUENCER_NUM_BEATS_PER_PATTERN];

  drum_step       dr_steps[SEQUENCER_NUM_DRUM_VOICES][SEQUENCER_NUM_STEPS_PER_PATTERN];
  instrument_step in_steps[SEQUENCER_NUM_INSTRUMENTS][SEQUENCER_NUM_STEPS_PER_PATTERN];

  int   num_bars;
} pattern;

extern pattern G_sequencer_pattern_bank[BANK_NUM_PATTERNS];

/* function declarations */
short int sequencer_pattern_setup_all();
short int sequencer_pattern_reset();

short int sequencer_setup();

short int sequencer_reset();

short int sequencer_activate_step();
short int sequencer_update();

short int sequencer_generate_tables();

#endif
