/*******************************************************************************
** Gersemi - Michael Behrens 2022
*******************************************************************************/

/*******************************************************************************
** main.c
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clock.h"
#include "datatree.h"
#include "downsamp.h"
#include "export.h"
#include "frame.h"
#include "linear.h"
#include "parse.h"
#include "sequence.h"
#include "sweep.h"
#include "synth.h"
#include "tuning.h"
#include "synth.h"
#include "waveform.h"

/*******************************************************************************
** main()
*******************************************************************************/
int main(int argc, char *argv[])
{
  int   i;

  char* name;
  char  input_filename[256];
  char  output_filename[256];

  data_tree_node* root;

  /* initialization */
  i = 0;

  name = NULL;
  root = NULL;

  /* read command line arguments */
  i = 1;

  while (i < argc)
  {
    /* name */
    if (!strcmp(argv[i], "-n"))
    {
      i++;
      if (i >= argc)
      {
        printf("Insufficient number of arguments. ");
        printf("Expected name. Exiting...\n");
        return 0;
      }

      name = strdup(argv[i]);
      i++;
    }
    else
    {
      printf("Unknown command line argument %s. Exiting...\n", argv[i]);
      return 0;
    }
  }

  /* make sure name is defined */
  if (name == NULL)
  {
    printf("Name not defined. Exiting...\n");
    return 0;
  }

  /* determine input and output filenames */
  strncpy(input_filename, name, 252);
  strncat(input_filename, ".txt", 4);

  strncpy(output_filename, name, 252);
  strncat(output_filename, ".wav", 4);

  /* setup */
  export_setup();
  frame_reset_buffer();
  downsamp_reset_buffer();

  /* initialize tables */
  envelope_generate_tables();
  lfo_generate_tables();
  linear_generate_tables();
  sweep_generate_tables();
  waveform_generate_tables();
  sequencer_generate_tables();

  /* more setup */
  synth_setup();
  sequencer_setup();

  tuning_setup();

  /* read input file */
  root = parse_file_to_data_tree(input_filename);

  if (root == NULL)
  {
    printf("Data tree not created from input file. Exiting...\n");
    goto cleanup;
  }

  parse_data_tree_to_globals(root);
  data_tree_node_destroy_tree(root);
  root = NULL;

  /* setup synth, reset sequencer */
  synth_load_patch(0, 0);
  synth_load_patch(1, 0);
  synth_load_patch(2, 0);
  synth_load_patch(3, 0);

  sequencer_reset();

  /* sound generation */
  export_write_to_file(output_filename);

  /* cleanup */
cleanup:

  return 0;
}
