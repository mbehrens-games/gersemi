/*******************************************************************************
** synth.c (individual synth)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "envelope.h"
#include "filter.h"
#include "lfo.h"
#include "patch.h"
#include "synth.h"
#include "voice.h"

patch   G_synth_patch_bank[SYNTH_MAX_PATCHES];

voice   G_synth_drum_voices[SYNTH_MAX_DRUM_VOICES];
voice   G_synth_sfx_voices[SYNTH_MAX_SFX_VOICES];
voice   G_synth_inst_voices[SYNTH_MAX_INST_VOICES];

int     G_synth_level;

/*******************************************************************************
** synth_setup()
*******************************************************************************/
short int synth_setup()
{
  int m;

  /* reset patch bank */
  for (m = 0; m < SYNTH_MAX_PATCHES; m++)
    patch_reset(&G_synth_patch_bank[m]);

  /* reset voice banks */
  for (m = 0; m < SYNTH_MAX_DRUM_VOICES; m++)
    voice_setup(&G_synth_drum_voices[m]);

  for (m = 0; m < SYNTH_MAX_SFX_VOICES; m++)
    voice_setup(&G_synth_sfx_voices[m]);

  for (m = 0; m < SYNTH_MAX_INST_VOICES; m++)
    voice_setup(&G_synth_inst_voices[m]);

  /* reset output level */
  G_synth_level = 0;

  return 0;
}

/*******************************************************************************
** synth_load_patch()
*******************************************************************************/
short int synth_load_patch(int voice_index, int patch_index)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* make sure patch index is valid */
  if ((patch_index < 0) || (patch_index >= SYNTH_MAX_PATCHES))
    return 1;

  /* load patch to this voice */
  voice_load_patch( &G_synth_inst_voices[voice_index], 
                    &G_synth_patch_bank[patch_index]);

  return 0;
}

/*******************************************************************************
** synth_set_vibrato()
*******************************************************************************/
short int synth_set_vibrato(int voice_index, int depth, int tempo, int speed)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* set vibrato in voice */
  voice_set_vibrato(&G_synth_inst_voices[voice_index], depth, tempo, speed);

  return 0;
}

/*******************************************************************************
** synth_set_tremolo()
*******************************************************************************/
short int synth_set_tremolo(int voice_index, int depth, int tempo, int speed)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* set tremolo in voice */
  voice_set_tremolo(&G_synth_inst_voices[voice_index], depth, tempo, speed);

  return 0;
}

/*******************************************************************************
** synth_set_wobble()
*******************************************************************************/
short int synth_set_wobble(int voice_index, int depth, int tempo, int speed)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* set extra lfo or suboscillator in voice */
  voice_set_wobble(&G_synth_inst_voices[voice_index], depth, tempo, speed);

  return 0;
}

/*******************************************************************************
** synth_set_pitch_sweep()
*******************************************************************************/
short int synth_set_pitch_sweep(int voice_index,  int mode, 
                                                  int tempo, 
                                                  int speed)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* set pitch sweep in voice */
  voice_set_pitch_sweep(&G_synth_inst_voices[voice_index], 
                        mode, tempo, speed);

  return 0;
}

/*******************************************************************************
** synth_key_on()
*******************************************************************************/
short int synth_key_on( int voice_index, 
                        int note, int volume, int brightness)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* send key on command to voice */
  voice_key_on(&G_synth_inst_voices[voice_index], note, volume, brightness);

  return 0;
}

/*******************************************************************************
** synth_key_off()
*******************************************************************************/
short int synth_key_off(int voice_index)
{
  /* make sure voice index is valid */
  if ((voice_index < 0) || (voice_index >= SYNTH_MAX_INST_VOICES))
    return 1;

  /* send key off command to voice */
  voice_key_off(&G_synth_inst_voices[voice_index]);

  return 0;
}

/*******************************************************************************
** synth_update()
*******************************************************************************/
short int synth_update()
{
  int m;

  int level;

  /* update voices */
  for (m = 0; m < SYNTH_MAX_INST_VOICES; m++)
    voice_update(&G_synth_inst_voices[m]);

  /* compute level */
  level = 0;

  for (m = 0; m < SYNTH_MAX_INST_VOICES; m++)
    level += G_synth_inst_voices[m].level;

  /* clipping */
  if (level > 32767)
    level = 32767;
  else if (level < -32768)
    level = -32768;

  /* set total level */
  G_synth_level = level;

  return 0;
}

