/*******************************************************************************
** export.c (file export functions)
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "downsamp.h"
#include "export.h"
#include "frame.h"

int G_export_sample_rate;
int G_export_bit_resolution;

static FILE*          S_export_fp;

static unsigned int   S_chunk_size;
static unsigned int   S_subchunk1_size;
static unsigned int   S_subchunk2_size;

static unsigned int   S_byte_rate;
static unsigned short S_audio_format;
static unsigned short S_block_align;

static unsigned int   S_sampling_rate;
static unsigned short S_bits_per_sample;
static unsigned short S_num_channels;

/*******************************************************************************
** export_setup()
*******************************************************************************/
short int export_setup()
{
  G_export_sample_rate = EXPORT_SAMPLE_RATE_36000;
  G_export_bit_resolution = EXPORT_BIT_RESOLUTION_16;

  S_export_fp = NULL;

  S_chunk_size = 0;
  S_subchunk1_size = 0;
  S_subchunk2_size = 0;

  S_byte_rate = 0;
  S_audio_format = 0;
  S_block_align = 0;

  S_sampling_rate = 0;
  S_bits_per_sample = 0;
  S_num_channels = 0;

  return 0;
}

/*******************************************************************************
** export_set_sample_rate()
*******************************************************************************/
short int export_set_sample_rate(int rate)
{
  /* make sure sample rate is valid */
  if ((rate < 0) || (rate >= EXPORT_NUM_SAMPLE_RATES))
    return 1;

  /* set sample rate */
  G_export_sample_rate = rate;

  return 0;
}

/*******************************************************************************
** export_set_bit_resolution()
*******************************************************************************/
short int export_set_bit_resolution(int res)
{
  /* make sure bit resolution is valid */
  if ((res < 0) || (res >= EXPORT_NUM_BIT_RESOLUTIONS))
    return 1;

  /* set bit resolution */
  G_export_bit_resolution = res;

  return 0;
}

/*******************************************************************************
** export_close_file()
*******************************************************************************/
short int export_close_file()
{
  if (S_export_fp != NULL)
  {
    fclose(S_export_fp);
    S_export_fp = NULL;
  }

  return 0;
}

/*******************************************************************************
** export_open_file()
*******************************************************************************/
short int export_open_file(char* filename)
{
  /* close file if one is currently open */
  if (S_export_fp != NULL)
    export_close_file();

  /* open file */
  S_export_fp = fopen(filename, "wb");

  /* if file did not open, return error */
  if (S_export_fp == NULL)
    return 1;

  return 0;
}

/*******************************************************************************
** export_write_header()
*******************************************************************************/
short int export_write_header()
{
  char id_field[4];

  /* make sure that file pointer is present */
  if (S_export_fp == NULL)
    return 1;

  /* set sampling rate */
  if (G_export_sample_rate == EXPORT_SAMPLE_RATE_36000)
    S_sampling_rate = 36000;
  else if (G_export_sample_rate == EXPORT_SAMPLE_RATE_22050)
    S_sampling_rate = 22050;
  else
    S_sampling_rate = 36000;

  /* set bits per sample */
  if (G_export_bit_resolution == EXPORT_BIT_RESOLUTION_16)
    S_bits_per_sample = 16;
  else if (G_export_bit_resolution == EXPORT_BIT_RESOLUTION_08)
    S_bits_per_sample = 8;
  else
    S_bits_per_sample = 16;

  /* set number of channels (mono) */
  S_num_channels = 1;

  /* compute subchunk sizes and other derived field values */
  S_audio_format = 1; /* 1 denotes PCM */
  S_block_align = S_num_channels * (S_bits_per_sample / 8);
  S_byte_rate = S_sampling_rate * S_block_align;

  S_subchunk1_size = 16; /* always 16 for PCM data */
  S_subchunk2_size = 0; /* eventually num_samples * block_align */
  S_chunk_size = 4 + (8 + S_subchunk1_size) + (8 + S_subchunk2_size);

  /* write 'RIFF' chunk */
  id_field[0] = 'R';
  id_field[1] = 'I';
  id_field[2] = 'F';
  id_field[3] = 'F';
  fwrite(id_field, 1, 4, S_export_fp);

  fwrite(&S_chunk_size, 4, 1, S_export_fp);

  id_field[0] = 'W';
  id_field[1] = 'A';
  id_field[2] = 'V';
  id_field[3] = 'E';
  fwrite(id_field, 1, 4, S_export_fp);

  /* write 'fmt ' chunk */
  id_field[0] = 'f';
  id_field[1] = 'm';
  id_field[2] = 't';
  id_field[3] = ' ';
  fwrite(id_field, 1, 4, S_export_fp);

  fwrite(&S_subchunk1_size, 4, 1, S_export_fp);
  fwrite(&S_audio_format, 2, 1, S_export_fp);
  fwrite(&S_num_channels, 2, 1, S_export_fp);
  fwrite(&S_sampling_rate, 4, 1, S_export_fp);
  fwrite(&S_byte_rate, 4, 1, S_export_fp);
  fwrite(&S_block_align, 2, 1, S_export_fp);
  fwrite(&S_bits_per_sample, 2, 1, S_export_fp);

  /* write 'data' chunk */
  id_field[0] = 'd';
  id_field[1] = 'a';
  id_field[2] = 't';
  id_field[3] = 'a';
  fwrite(id_field, 1, 4, S_export_fp);

  fwrite(&S_subchunk2_size, 4, 1, S_export_fp);

  return 0;
}

/*******************************************************************************
** export_write_block()
*******************************************************************************/
short int export_write_block(short int* buffer, int num_samples)
{
  int           i;
  unsigned char temp_char;

  /* make sure that file pointer is present */
  if (S_export_fp == NULL)
    return 1;

  /* make sure buffer is present */
  if (buffer == NULL)
    return 1;

  /* make sure number of samples is positive */
  if (num_samples <= 0)
    return 1;

  /* make sure position is the end of the file */
  fseek(S_export_fp, 0, SEEK_END);

  /* write 8-bit mono data */
  if (G_export_bit_resolution == EXPORT_BIT_RESOLUTION_08)
  {
    for (i = 0; i < num_samples; i++)
    {
      temp_char = 127 - (buffer[i] / 256);
      fwrite(&temp_char, 1, 1, S_export_fp);
    }
  }
  /* write 16-bit mono data */
  else if (G_export_bit_resolution == EXPORT_BIT_RESOLUTION_16)
  {
    fwrite(buffer, 2, num_samples, S_export_fp);
  }
  /* otherwise, return error */
  else
    return 1;

  return 0;
}

/*******************************************************************************
** export_update_header()
*******************************************************************************/
short int export_update_header(unsigned int frame_num)
{
  int num_samples;

  /* compute number of samples generated */
  if (G_export_sample_rate == EXPORT_SAMPLE_RATE_36000)
    num_samples = frame_num * FRAME_SAMPLES_PER_FRAME;
  else if (G_export_sample_rate == EXPORT_SAMPLE_RATE_22050)
  {
    num_samples = (frame_num / 2) * DOWNSAMP_SAMPLES_PER_TWO_FRAMES + 
                  (frame_num % 2) * DOWNSAMP_SAMPLES_PER_ODD_FRAME;
  }
  else
    num_samples = frame_num * FRAME_SAMPLES_PER_FRAME;

  /* compute subchunk2 size */
  S_subchunk2_size = num_samples * S_block_align;
  S_chunk_size = 4 + (8 + S_subchunk1_size) + (8 + S_subchunk2_size);

  /* write updated chunk size */
  fseek(S_export_fp, 4, SEEK_SET);
  fwrite(&S_chunk_size, 4, 1, S_export_fp);

  /* write updated subchunk2 size */
  fseek(S_export_fp, 40, SEEK_SET);
  fwrite(&S_subchunk2_size, 4, 1, S_export_fp);

  return 0;
}

/*******************************************************************************
** export_write_to_file()
*******************************************************************************/
short int export_write_to_file(char* filename)
{
  int m;
  unsigned int frame_num;

  /* make sure filename is valid */
  if (filename == NULL)
    return 1;

  /* open file */
  export_open_file(filename);

  /* write header */
  export_write_header();

  /* generate frames of samples and write them to the file */
  frame_num = 0;

  frame_reset_buffer();
  frame_prepare_for_playback();

  if (G_export_sample_rate == EXPORT_SAMPLE_RATE_22050)
    downsamp_reset_buffer();

  /* just testing for now; generate 120 frames (2 seconds) */
  for (m = 0; m < 120; m++)
  {
    frame_generate_one_frame();

    if (G_export_sample_rate == EXPORT_SAMPLE_RATE_22050)
    {
      downsamp_downsample_one_frame(frame_num);

      if (frame_num % 2 == 0)
        export_write_block(G_downsamp_sample_buffer, DOWNSAMP_SAMPLES_PER_EVEN_FRAME);
      else
        export_write_block(G_downsamp_sample_buffer, DOWNSAMP_SAMPLES_PER_ODD_FRAME);
    }
    else
      export_write_block(G_frame_sample_buffer, FRAME_SAMPLES_PER_FRAME);

    frame_num += 1;
  }

  /* update header based on frames written */
  export_update_header(frame_num);

  /* close file */
  export_close_file();

  return 0;
}

