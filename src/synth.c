/*******************************************************************************
** synth.c (the synth!)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "envelope.h"
#include "filter.h"
#include "instrument.h"
#include "lfo.h"
#include "peg.h"
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
  peg_reset_all();
  voice_reset_all();

  arpeggio_reset_all();
  sweep_reset_all();

  instrument_reset_all();

  cart_reset_all();

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
  int n;

  instrument* ins;
  voice* v;

  envelope* e;
  lfo* l;
  peg* pg;
  sweep* sw;

  filter* hpf;

  short int voice_index;

  int level;

  /* update arpeggios */
  arpeggio_update_all();

  /* update sweeps */
  sweep_update_all();

  /* update lfos */
  lfo_update_all();

  /* update pitch envelopes */
  peg_update_all();

  /* copy lfo & sweep levels to envelope & voice inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    for (m = 0; m < ins->polyphony; m++)
    {
      voice_index = ins->voice_index + m;

      v = &G_voice_bank[voice_index];

      /* vibrato */
      l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + 0];

      v->vibrato_base = l->level_base;
      v->vibrato_extra = l->level_extra;

      /* tremolo */
      l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + 1];

      for (n = 0; n < BANK_ENVELOPES_PER_VOICE; n++)
      {
        e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + n];

        e->tremolo_base = l->level_base;
        e->tremolo_extra = l->level_extra;
      }

      /* chorus */
      l = &G_lfo_bank[voice_index * BANK_LFOS_PER_VOICE + 2];

      v->chorus_base = l->level_base;
      v->chorus_extra = l->level_extra;

      /* sweep */
      sw = &G_sweep_bank[voice_index];

      v->sweep_input = sw->level;

      /* pitch envelope */
      pg = &G_peg_bank[voice_index];

      v->peg_input = pg->level;
    }
  }

  /* update envelopes */
  envelope_update_all();

  /* copy envelope levels to voice inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    for (m = 0; m < ins->polyphony; m++)
    {
      voice_index = ins->voice_index + m;

      v = &G_voice_bank[voice_index];

      for (n = 0; n < BANK_ENVELOPES_PER_VOICE; n++)
      {
        e = &G_envelope_bank[voice_index * BANK_ENVELOPES_PER_VOICE + n];

        v->env_input[n] = e->level;
      }
    }
  }

  /* update voices */
  voice_update_all();

  /* copy voice levels to highpass filter inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    for (m = 0; m < ins->polyphony; m++)
    {
      voice_index = ins->voice_index + m;

      v = &G_voice_bank[voice_index];
      hpf = &G_highpass_filter_bank[voice_index];

      hpf->input = v->level;
    }
  }

  /* update filter sets */
  filter_update_all();

  /* compute overall level */
  level = 0;

  for (k = 0; k < BANK_NUM_FILTER_SETS; k++)
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
  sweep_generate_tables();
  arpeggio_generate_tables();

  voice_generate_tables();
  envelope_generate_tables();
  peg_generate_tables();
  lfo_generate_tables();
  filter_generate_tables();

  return 0;
}

