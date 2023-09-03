/*******************************************************************************
** sequence.c (sequencer)
*******************************************************************************/

#include <stdio.h>  /* testing */
#include <stdlib.h>
#include <math.h>

#include "clock.h"
#include "sequence.h"
#include "synth.h"

/* internal sequencer variables */
static unsigned int S_sequencer_music_phase;
static unsigned int S_sequencer_music_increment;

/*******************************************************************************
** sequencer_setup()
*******************************************************************************/
short int sequencer_setup()
{
  S_sequencer_music_phase = 0;
  S_sequencer_music_increment = 0;

  return 0;
}

/*******************************************************************************
** sequencer_reset()
*******************************************************************************/
short int sequencer_reset()
{
  S_sequencer_music_phase = 0;
  S_sequencer_music_increment = 0;

  return 0;
}

/*******************************************************************************
** sequencer_update()
*******************************************************************************/
short int sequencer_update()
{
  /* if all midi events played, return... */

  /* update phase */
  S_sequencer_music_phase += S_sequencer_music_increment;

  /* check if a sequencer tick was completed */
  if (S_sequencer_music_phase > 0xFFFFFFF)
  {
    S_sequencer_music_phase &= 0xFFFFFFF;

    /* activate the event (key on, key off, set mod wheel, etc...) */
  }

  return 0;
}

