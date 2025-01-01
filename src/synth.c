/*******************************************************************************
** synth.c (the synth!)
*******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "bank.h"
#include "cart.h"
#include "clock.h"
#include "filter.h"
#include "instrument.h"
#include "sequence.h"
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
  voice_reset_all();
  filter_reset_all();

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

  int level;

  /* update voices */
  voice_update_all();

  /* copy voice levels to filter inputs */
  for (k = 0; k < BANK_NUM_VOICES; k++)
    G_filter_bank[k].input = G_voice_bank[k].level;

  /* update filter sets */
  filter_update_all();

  /* compute overall level from filter outputs */
  level = 0;

  for (k = 0; k < BANK_NUM_VOICES; k++)
    level += G_filter_bank[k].level;

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
  voice_generate_tables();
  filter_generate_tables();

  return 0;
}

