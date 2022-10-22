/*******************************************************************************
** waveform.c (oscillator wavetables)
*******************************************************************************/

#include <stdio.h>
#include <math.h>

#include "pi.h"
#include "waveform.h"

static short int S_db_to_linear_table[4096];

static short int S_wavetable_sine[512];
static short int S_wavetable_tri[512];
static short int S_wavetable_saw[1024];
static short int S_wavetable_phat_saw[1024];

/*******************************************************************************
** waveform_generate_tables()
*******************************************************************************/
short int waveform_generate_tables()
{
  int     i;
  double  val;

  /* ym2612 - 10 bit envelope (shifted to 12 bit), 12 bit sine, 13 bit sum    */
  /* 10 bit db: 24, 12, 6, 3, 1.5, 0.75, 0.375, 0.1875, 0.09375, 0.046875     */
  /* 12 bit db: adds on 0.0234375, 0.01171875 in back                         */
  /* 13 bit db: adds on 48 in front                                           */

  /* db to linear scale conversion */
  S_db_to_linear_table[0] = 32767;

  for (i = 1; i < 4095; i++)
  {
    S_db_to_linear_table[i] = 
      (short int) ((32767.0f * exp(-log(10) * (DB_STEP_12_BIT / 10) * i)) + 0.5f);
  }

  S_db_to_linear_table[4095] = 0;

  /* wavetable (sine) */
  S_wavetable_sine[0] = 4095;
  S_wavetable_sine[256] = 0;

  for (i = 1; i < 256; i++)
  {
    val = sin(TWO_PI * (i / 1024.0f));
    S_wavetable_sine[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_wavetable_sine[512 - i] = S_wavetable_sine[i];
  }

  /* wavetable (triangle) */
  S_wavetable_tri[0] = 4095;
  S_wavetable_tri[256] = 0;

  for (i = 1; i < 256; i++)
  {
    val = i / 256.0f;
    S_wavetable_tri[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_wavetable_tri[512 - i] = S_wavetable_tri[i];
  }

  /* wavetable (sawtooth) */
  S_wavetable_saw[0] = 4095;
  S_wavetable_saw[512] = 0;

  for (i = 1; i < 512; i++)
  {
    val = i / 512.0f;
    S_wavetable_saw[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
    S_wavetable_saw[1024 - i] = S_wavetable_saw[i];
  }

  /* wavetable (phat saw) */
  S_wavetable_phat_saw[0] = 257;
  S_wavetable_phat_saw[256] = 4095;
  S_wavetable_phat_saw[512] = 257;
  S_wavetable_phat_saw[768] = 0;

  for (i = 1; i < 256; i++)
  {
    val = 0.5f + (i / 512.0f);
    S_wavetable_phat_saw[i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);

    val = i / 512.0f;
    S_wavetable_phat_saw[256 + i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);

    val = 0.5f - (i / 512.0f);
    S_wavetable_phat_saw[512 + i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);

    val = 1.0f - (i / 512.0f);
    S_wavetable_phat_saw[768 + i] = (short int) ((10 * (log(1 / val) / log(10)) / DB_STEP_12_BIT) + 0.5f);
  }

  return 0;
}

/*******************************************************************************
** waveform_wave_lookup()
*******************************************************************************/
int waveform_wave_lookup( int waveform, 
                          unsigned int shifted_phase, 
                          int env_index)
{
  int final_index;
  int level;

  /* bound phase */
  shifted_phase &= 0x3FF;

  /* determine final index */
  if (waveform == WAVEFORM_NONE)
    final_index = 4095;
  else if (waveform == WAVEFORM_SINE)
  {
    if (shifted_phase < 512)
      final_index = S_wavetable_sine[shifted_phase];
    else
      final_index = S_wavetable_sine[shifted_phase - 512];
  }
  else if (waveform == WAVEFORM_SQUARE)
    final_index = 0;
  else if (waveform == WAVEFORM_TRIANGLE)
  {
    if (shifted_phase < 512)
      final_index = S_wavetable_tri[shifted_phase];
    else
      final_index = S_wavetable_tri[shifted_phase - 512];
  }
  else if (waveform == WAVEFORM_SAW)
    final_index = S_wavetable_saw[shifted_phase];
  else if (waveform == WAVEFORM_PHAT_SAW)
    final_index = S_wavetable_phat_saw[shifted_phase];
  else
    final_index = 4095;

  /* apply shifted envelope index */
  final_index += (env_index << 2);

  /* bound final index */
  if (final_index < 0)
    final_index = 0;
  else if (final_index > 4095)
    final_index = 4095;

  /* compute output level and return it! */
  level = 0;

  if (shifted_phase < 512)
    level = S_db_to_linear_table[final_index];
  else
    level = -S_db_to_linear_table[final_index];

  return level;
}

/*******************************************************************************
** waveform_pulse_lookup()
*******************************************************************************/
int waveform_pulse_lookup(int width, 
                          unsigned int shifted_phase, 
                          int env_index)
{
  unsigned int falling_edge;

  int final_index;
  int level;

  /* initialize final index */
  final_index = 0;

  /* apply shift to envelope index */
  final_index += (env_index << 2);

  /* bound final index */
  if (final_index < 0)
    final_index = 0;
  else if (final_index > 4095)
    final_index = 4095;

  /* determine falling edge position */
  falling_edge = 512 - width;

  /* bound falling edge position */
  if (falling_edge < 64)
    falling_edge = 64;
  else if (falling_edge > 512)
    falling_edge = 512;

  /* determine level */
  if (shifted_phase < falling_edge)
    level = S_db_to_linear_table[final_index];
  else
    level = -S_db_to_linear_table[final_index];

  return level;
}

/*******************************************************************************
** waveform_noise_lookup()
*******************************************************************************/
int waveform_noise_lookup(short int alternate, 
                          unsigned int lfsr, 
                          int env_index)
{
  int final_index;
  int level;

  /* determine final index */
  if (alternate == 1)
    final_index = S_wavetable_saw[lfsr & 0x3FF];
  else
    final_index = 0;

  /* apply shift to envelope index */
  final_index += (env_index << 2);

  /* bound final index */
  if (final_index < 0)
    final_index = 0;
  else if (final_index > 4095)
    final_index = 4095;

  /* determine level */
  if ((lfsr & 0x3FF) < 512)
    level = S_db_to_linear_table[final_index];
  else
    level = -S_db_to_linear_table[final_index];

  return level;
}

