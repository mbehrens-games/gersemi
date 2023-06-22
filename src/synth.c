/*******************************************************************************
** synth.c (individual synth)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "clock.h"
#include "envelope.h"
#include "filter.h"
#include "key.h"
#include "lfo.h"
#include "patch.h"
#include "sweep.h"
#include "synth.h"
#include "tuning.h"
#include "voice.h"

int G_synth_level_left;
int G_synth_level_right;

/*******************************************************************************
** synth_generate_tables()
*******************************************************************************/
short int synth_generate_tables()
{
  envelope_generate_tables();
  key_generate_tables();
  lfo_generate_tables();
  sweep_generate_tables();
  voice_generate_tables();

  return 0;
}

/*******************************************************************************
** synth_reset_banks()
*******************************************************************************/
short int synth_reset_banks()
{
  /* reset all banks */
  envelope_setup_all();
  key_setup_all();
  filter_setup_all();
  lfo_setup_all();
  patch_setup_all();
  sweep_setup_all();
  voice_setup_all();

  /* reset tuning tables */
  tuning_reset();

  /* reset output levels */
  G_synth_level_left = 0;
  G_synth_level_right = 0;

  return 0;
}

/*******************************************************************************
** synth_load_patch()
*******************************************************************************/
short int synth_load_patch(int voice_index, int patch_index)
{
  voice* v;
  patch* p;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* make sure that the patch index is valid */
  if (BANK_PATCH_INDEX_IS_NOT_VALID(patch_index))
    return 1;

  /* obtain voice and patch pointers */
  v = &G_voice_bank[voice_index];
  p = &G_patch_bank[patch_index];

  /* voice */
  voice_load_patch(voice_index, patch_index);

  /* envelopes */
  if ((v->algorithm == VOICE_ALGORITHM_1_CAR_CHAIN) || 
      (v->algorithm == VOICE_ALGORITHM_1_CAR_Y)     || 
      (v->algorithm == VOICE_ALGORITHM_1_CAR_CRAB_CLAW))
  {
    envelope_load_patch(voice_index, 0, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 1, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 2, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 3, patch_index, ENVELOPE_TYPE_CARRIER);
  }
  else if ( (v->algorithm == VOICE_ALGORITHM_2_CAR_TWIN) || 
            (v->algorithm == VOICE_ALGORITHM_2_CAR_STACKED))
  {
    envelope_load_patch(voice_index, 0, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 1, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 2, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 3, patch_index, ENVELOPE_TYPE_CARRIER);
  }
  else if ( (v->algorithm == VOICE_ALGORITHM_3_CAR_ONE_TO_THREE) || 
            (v->algorithm == VOICE_ALGORITHM_3_CAR_ONE_TO_ONE))
  {
    envelope_load_patch(voice_index, 0, patch_index, ENVELOPE_TYPE_MODULATOR);
    envelope_load_patch(voice_index, 1, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 2, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 3, patch_index, ENVELOPE_TYPE_CARRIER);
  }
  else if (v->algorithm == VOICE_ALGORITHM_4_CAR_PIPES)
  {
    envelope_load_patch(voice_index, 0, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 1, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 2, patch_index, ENVELOPE_TYPE_CARRIER);
    envelope_load_patch(voice_index, 3, patch_index, ENVELOPE_TYPE_CARRIER);
  }

  /* lfos */

  /* filters */
  filter_set_lowpass_cutoff(voice_index, p->lowpass_cutoff);
  filter_set_highpass_cutoff(voice_index, p->highpass_cutoff);

  return 0;
}

/*******************************************************************************
** synth_key_on()
*******************************************************************************/
short int synth_key_on( int voice_index, 
                        int octave, int degree, 
                        int volume, int brightness)
{
  int m;

  int note;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* lookup this note */
  note = key_note_lookup(voice_index, octave, degree);

  /* set voice note */
  voice_set_note(voice_index, note);

  /* trigger envelopes */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    envelope_trigger(voice_index, m, v->osc_note[m], volume, brightness);

  return 0;
}

/*******************************************************************************
** synth_key_off()
*******************************************************************************/
short int synth_key_off(int voice_index)
{
  int m;

  voice* v;

  /* make sure that the voice index is valid */
  if (BANK_VOICE_INDEX_IS_NOT_VALID(voice_index))
    return 1;

  /* obtain voice pointer */
  v = &G_voice_bank[voice_index];

  /* release envelopes */
  for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
    envelope_release(voice_index, m);

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

  /* update lfos */
  lfo_update_all();

  /* update sweeps */
  sweep_update_all();

  /* update envelopes */
  envelope_update_all();

  /* copy envelope levels to voice inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
  {
    for (m = 0; m < VOICE_NUM_OSCS_AND_ENVS; m++)
      G_voice_bank[k].env_input[m] = G_envelope_bank[4 * k + m].level;
  }

  /* update voices */
  voice_update_all();

  /* copy voice levels to lowpass filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_filter_bank[2 * k + 0].input = G_voice_bank[k].level;

  /* update filters */
  filter_update_all();

  /* compute overall level */
  level = 0;

  for (k = 0; k < BANK_NUM_VOICES; k++)
    level += G_filter_bank[2 * k + 1].level;

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

