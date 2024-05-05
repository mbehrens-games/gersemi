/*******************************************************************************
** synth.c (the synth!)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "arpeggio.h"
#include "clock.h"
#include "envelope.h"
#include "highpass.h"
#include "instrument.h"
#include "lfo.h"
#include "lowpass.h"
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
  highpass_reset_all();
  lfo_reset_all();
  lowpass_reset_all();
  voice_reset_all();

  arpeggio_reset_all();
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
  highpass* hpf;
  lowpass* lpf;

  /* update arpeggios */
  arpeggio_update_all();

  /* update sweeps */
  sweep_update_all();

  /* update lfos */
  lfo_update_all();

  /* update envelopes */
  envelope_update_all();

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

        v->env_1_input = G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + 0].level;
        v->env_2_input = G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + 1].level;
        v->env_3_input = G_envelope_bank[(ins->voice_index + m) * BANK_ENVELOPES_PER_VOICE + 2].level;

        v->sweep_input = G_sweep_bank[k].level;
      }
    }
    else if (ins->type == INSTRUMENT_TYPE_MONO)
    {
      v = &G_voice_bank[ins->voice_index];

#if 0
      v->lfo_input_vibrato = G_lfo_bank[ins->voice_index].vibrato_level;
      v->lfo_input_tremolo = G_lfo_bank[ins->voice_index].tremolo_level;
#endif

      v->env_1_input = G_envelope_bank[ins->voice_index * BANK_ENVELOPES_PER_VOICE + 0].level;
      v->env_2_input = G_envelope_bank[ins->voice_index * BANK_ENVELOPES_PER_VOICE + 1].level;
      v->env_3_input = G_envelope_bank[ins->voice_index * BANK_ENVELOPES_PER_VOICE + 2].level;

      v->sweep_input = G_sweep_bank[k].level;
    }
  }

  /* update voices */
  voice_update_all();

  /* copy voice levels to lowpass filter inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    if (ins->type == INSTRUMENT_TYPE_POLY)
    {
      for (m = 0; m < BANK_VOICES_PER_POLY_INSTRUMENT; m++)
      {
        v = &G_voice_bank[ins->voice_index + m];
        lpf = &G_lowpass_bank[ins->voice_index + m];

        lpf->input = v->level;
      }
    }
    else if (ins->type == INSTRUMENT_TYPE_MONO)
    {
      v = &G_voice_bank[ins->voice_index];
      lpf = &G_lowpass_bank[ins->voice_index];

      lpf->input = v->level;
    }
  }

  /* update lowpass filters */
  lowpass_update_all();

  /* copy lowpass filter outputs to highpass filter inputs */
  for (k = 0; k < BANK_NUM_INSTRUMENTS; k++)
  {
    ins = &G_instrument_bank[k];

    if (ins->type == INSTRUMENT_TYPE_POLY)
    {
      for (m = 0; m < BANK_VOICES_PER_POLY_INSTRUMENT; m++)
      {
        lpf = &G_lowpass_bank[ins->voice_index + m];
        hpf = &G_highpass_bank[ins->voice_index + m];

        hpf->input = lpf->level;
      }
    }
    else if (ins->type == INSTRUMENT_TYPE_MONO)
    {
      lpf = &G_lowpass_bank[ins->voice_index];
      hpf = &G_highpass_bank[ins->voice_index];

      hpf->input = lpf->level;
    }
  }

  /* update highpass filters */
  highpass_update_all();

  /* compute overall level */
  level = 0;

  for (k = 0; k < BANK_NUM_HIGHPASSES; k++)
    level += G_highpass_bank[k].level;

  /* clipping */
  if (level > 32767)
    level = 32767;
  else if (level < -32768)
    level = -32768;

  /* set total levels */
  G_synth_level_left = level;
  G_synth_level_right = level;

#if 0
  printf("Synth Level: %d %d\n", G_synth_level_left, G_synth_level_right);
#endif

  return 0;
}

/*******************************************************************************
** synth_generate_tables()
*******************************************************************************/
short int synth_generate_tables()
{
  arpeggio_generate_tables();
  envelope_generate_tables();
  highpass_generate_tables();
  lfo_generate_tables();
  lowpass_generate_tables();
  sweep_generate_tables();
  voice_generate_tables();

  return 0;
}

