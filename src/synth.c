/*******************************************************************************
** synth.c (the synth!)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "bender.h"
#include "boost.h"
#include "clock.h"
#include "envelope.h"
#include "filter.h"
#include "instrument.h"
#include "lfo.h"
#include "patch.h"
#include "sequence.h"
#include "sweep.h"
#include "synth.h"
#include "tuning.h"
#include "voice.h"

int G_synth_level_left;
int G_synth_level_right;

/*******************************************************************************
** synth_reset_banks()
*******************************************************************************/
short int synth_reset_banks()
{
  /* reset all banks */
  envelope_reset_all();
  filter_reset_all();
  lfo_reset_all();
  voice_reset_all();

  arpeggio_reset_all();
  boost_reset_all();
  bender_reset_all();
  sweep_reset_all();

  instrument_reset_all();

  patch_reset_all();

  /* reset tuning table */
  tuning_reset();

  /* reset output levels */
  G_synth_level_left = 0;
  G_synth_level_right = 0;

  return 0;
}

/*******************************************************************************
** synth_update()
*******************************************************************************/
short int synth_update()
{
  int k;
  int m;

  int level;

  instrument* ins;
  voice* v;

  /* update arpeggios */
  arpeggio_update_all();

  /* update boosts */
  boost_update_all();

  /* update benders */
  bender_update_all();

  /* update sweeps */
  sweep_update_all();

  /* update envelopes */
  envelope_update_all();

  /* update lfos */
  lfo_update_all();

  /* copy levels to voice inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    if (ins->type == INSTRUMENT_TYPE_POLY)
    {
      for (m = 0; m < BANK_VOICES_PER_POLY_INSTRUMENT; m++)
      {
        v = &G_voice_bank[ins->voice_index + m];

#if 0
        v->lfo_input_vibrato = G_lfo_bank[ins->voice_index + m].vibrato_level;
        v->lfo_input_tremolo = G_lfo_bank[ins->voice_index + m].tremolo_level;
#endif

        v->env_input = G_envelope_bank[ins->voice_index + m].level;

        v->boost_input = G_boost_bank[k].level;
        v->sweep_input = G_sweep_bank[k].level;
        v->bender_input = G_bender_bank[k].level;
      }
    }
    else if (ins->type == INSTRUMENT_TYPE_MONO)
    {
      v = &G_voice_bank[ins->voice_index];

#if 0
      v->lfo_input_vibrato = G_lfo_bank[ins->voice_index].vibrato_level;
      v->lfo_input_tremolo = G_lfo_bank[ins->voice_index].tremolo_level;
#endif

      v->env_input = G_envelope_bank[ins->voice_index].level;

      v->boost_input = G_boost_bank[k].level;
      v->sweep_input = G_sweep_bank[k].level;
      v->bender_input = G_bender_bank[k].level;
    }
  }

  /* update voices */
  voice_update_all();

#if 0
  /* testing */
  printf("Voice Levels: ");

  for (k = 0; k < BANK_NUM_VOICES; k++)
    printf("%d ", G_voice_bank[k].level);

  printf("\n");
#endif

#if 0
  /* copy voice levels to lowpass filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_lowpass_filter_bank[k].input = G_voice_bank[k].level;

  /* update lowpass filters */
  filter_update_lowpass();

  /* copy lowpass filter outputs to highpass filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_highpass_filter_bank[k].input = G_lowpass_filter_bank[k].level;

  /* update highpass filters */
  filter_update_highpass();
#endif

  /* copy voice levels to highpass filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_highpass_filter_bank[k].input = G_voice_bank[k].level;

  /* update highpass filters */
  filter_update_highpass();

  /* copy highpass filter outputs to lowpass filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_lowpass_filter_bank[k].input = G_highpass_filter_bank[k].level;

  /* update lowpass filters */
  filter_update_lowpass();

  /* compute overall level */
  level = 0;

  for (k = 0; k < BANK_NUM_VOICES; k++)
    level += G_lowpass_filter_bank[k].level;

  /* clipping */
  if (level > 32767)
    level = 32767;
  else if (level < -32768)
    level = -32768;

  /* set total levels */
  G_synth_level_left = level;
  G_synth_level_right = level;

  return 0;
}

/*******************************************************************************
** synth_generate_tables()
*******************************************************************************/
short int synth_generate_tables()
{
  arpeggio_generate_tables();
  envelope_generate_tables();
  filter_generate_tables();
  lfo_generate_tables();
  sweep_generate_tables();
  voice_generate_tables();

  return 0;
}

