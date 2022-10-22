/*******************************************************************************
** waveform.h (oscillator wavetables)
*******************************************************************************/

#ifndef WAVEFORM_H
#define WAVEFORM_H

#define DB_STEP_12_BIT 0.01171875f

enum
{
  WAVEFORM_NONE = 0,
  WAVEFORM_SINE,
  WAVEFORM_SQUARE,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SAW,
  WAVEFORM_PHAT_SAW
};

/* function declarations */
short int waveform_generate_tables();

int waveform_wave_lookup( int waveform, 
                          unsigned int shifted_phase, 
                          int env_index);

int waveform_pulse_lookup(int width, 
                          unsigned int shifted_phase, 
                          int env_index);

int waveform_noise_lookup(short int alternate, 
                          unsigned int lfsr, 
                          int env_index);

#endif
