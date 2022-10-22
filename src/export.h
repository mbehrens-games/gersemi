/*******************************************************************************
** export.h (file export functions)
*******************************************************************************/

#ifndef EXPORT_H
#define EXPORT_H

enum
{
  EXPORT_SAMPLE_RATE_36000 = 0,
  EXPORT_SAMPLE_RATE_22050,
  EXPORT_NUM_SAMPLE_RATES
};

enum
{
  EXPORT_BIT_RESOLUTION_16 = 0,
  EXPORT_BIT_RESOLUTION_08,
  EXPORT_NUM_BIT_RESOLUTIONS
};

extern int G_export_sample_rate;
extern int G_export_bit_resolution;

/* function declarations */
short int export_setup();

short int export_set_sample_rate(int rate);
short int export_set_bit_resolution(int res);

short int export_write_to_file(char* filename);

#endif
