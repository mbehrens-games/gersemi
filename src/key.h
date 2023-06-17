/*******************************************************************************
** key.h (musical keys)
*******************************************************************************/

#ifndef KEY_H
#define KEY_H

#include "bank.h"

#if 0
enum
{
  /* major keys */
  KEY_SIGNATURE_C_FLAT_MAJOR = 0, /* 7 flats  */
  KEY_SIGNATURE_G_FLAT_MAJOR,
  KEY_SIGNATURE_D_FLAT_MAJOR,
  KEY_SIGNATURE_A_FLAT_MAJOR,
  KEY_SIGNATURE_E_FLAT_MAJOR,
  KEY_SIGNATURE_B_FLAT_MAJOR,
  KEY_SIGNATURE_F_MAJOR,
  KEY_SIGNATURE_C_MAJOR,          /* clear    */
  KEY_SIGNATURE_G_MAJOR,
  KEY_SIGNATURE_D_MAJOR,
  KEY_SIGNATURE_A_MAJOR,
  KEY_SIGNATURE_E_MAJOR,
  KEY_SIGNATURE_B_MAJOR,
  KEY_SIGNATURE_F_SHARP_MAJOR,
  KEY_SIGNATURE_C_SHARP_MAJOR,    /* 7 sharps */
  /* minor keys */
  KEY_SIGNATURE_A_FLAT_MINOR,     /* 7 flats  */
  KEY_SIGNATURE_E_FLAT_MINOR,
  KEY_SIGNATURE_B_FLAT_MINOR,
  KEY_SIGNATURE_F_MINOR,
  KEY_SIGNATURE_C_MINOR,
  KEY_SIGNATURE_G_MINOR,
  KEY_SIGNATURE_D_MINOR,
  KEY_SIGNATURE_A_MINOR,          /* clear    */
  KEY_SIGNATURE_E_MINOR,
  KEY_SIGNATURE_B_MINOR,
  KEY_SIGNATURE_F_SHARP_MINOR,
  KEY_SIGNATURE_C_SHARP_MINOR,
  KEY_SIGNATURE_G_SHARP_MINOR,
  KEY_SIGNATURE_D_SHARP_MINOR,
  KEY_SIGNATURE_A_SHARP_MINOR,    /* 7 sharps */
  KEY_NUM_SIGNATURES
};
#endif

typedef struct key
{
  int sig;
  int mode;
} key;

extern key G_key_signature_bank[BANK_NUM_KEY_SETTINGS];
extern key G_key_modulation_bank[BANK_NUM_KEY_SETTINGS];

/* function declarations */
short int key_setup_all();
short int key_reset(int voice_index);

int       key_note_lookup(int voice_index, int octave, int degree);

short int key_generate_tables();

#endif
