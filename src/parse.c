/*******************************************************************************
** parse.c (parsing functions)
*******************************************************************************/

#include <stdio.h>    /* testing */
#include <stdlib.h>
#include <string.h>

#include "datatree.h"
#include "export.h"
#include "lfo.h"
#include "patch.h"
#include "parse.h"
#include "sequence.h"
#include "synth.h"
#include "tempo.h"
#include "token.h"
#include "tuning.h"
#include "waveform.h"

#define PARSE_EAT_TOKEN(just_eat_it)                                           \
  if (t.token == just_eat_it)                                                  \
    tokenizer_advance(&t);                                                     \
  else                                                                         \
    goto houston;

/*******************************************************************************
** parse_file_to_data_tree()
*******************************************************************************/
data_tree_node* parse_file_to_data_tree(char* filename)
{
  tokenizer         t;
  data_tree_node*   root;
  data_tree_node*   current;
  data_tree_node**  stack;
  int               stack_size;
  int               stack_top;
  int               parse_state;

  /* initialize tokenizer and open file */
  tokenizer_init(&t);

  if (tokenizer_open_file(&t, filename))
    return NULL;

  /* setup stack */
  stack = malloc(DATA_TREE_STACK_INITIAL_SIZE * sizeof(data_tree_node*));
  stack_size = DATA_TREE_STACK_INITIAL_SIZE;
  stack_top = -1;

  /* initial parsing; create root node and push onto stack */
  root = data_tree_node_create();

  PARSE_EAT_TOKEN(TOKEN_LESS_THAN)

  if ((t.token == TOKEN_IDENTIFIER) && (!strcmp(t.sb, "gersemi")))
  {
    root->type = DATA_TREE_NODE_TYPE_FIELD_GERSEMI;
    current = root;
    DATA_TREE_PUSH_NODE(stack, root)
    tokenizer_advance(&t);
  }
  else
    goto cleanup;

  parse_state = PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE;

  /* begin parsing subfields */
  while (stack_top >= 0)
  {
    /* attribute */
    if ((t.token == TOKEN_AT_SYMBOL) && 
        (parse_state == PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE))
    {
      DATA_TREE_CREATE_NEW_NODE(stack, current)
      tokenizer_advance(&t);

      if (t.token != TOKEN_IDENTIFIER)
        goto houston;

      if (!strcmp(t.sb, "export_sampling"))
        current->type = DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_SAMPLING;
      else if (!strcmp(t.sb, "export_bitres"))
        current->type = DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_BITRES;
      else if (!strcmp(t.sb, "tuning_system"))
        current->type = DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_SYSTEM;
      else if (!strcmp(t.sb, "tuning_fork"))
        current->type = DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_FORK;
      else
        goto houston;

      tokenizer_advance(&t);

      PARSE_EAT_TOKEN(TOKEN_EQUAL_SIGN)

      if (t.token == TOKEN_NUMBER_INTEGER)
      {
        current->child = data_tree_node_create();
        current->child->type = DATA_TREE_NODE_TYPE_VALUE_INTEGER;
        current->child->value = strdup(t.sb);
      }
      else if (t.token == TOKEN_STRING)
      {
        current->child = data_tree_node_create();
        current->child->type = DATA_TREE_NODE_TYPE_VALUE_STRING;
        current->child->value = strdup(t.sb);
      }
      else
        goto houston;

      tokenizer_advance(&t);
    }
    /* subfield */
    else if ( (t.token == TOKEN_LESS_THAN) &&
              ( (parse_state == PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE) ||
                (parse_state == PARSE_STATE_SUBFIELD_OR_END_OF_FIELD)))
    {
      DATA_TREE_CREATE_NEW_NODE(stack, current)
      tokenizer_advance(&t);

      if (t.token != TOKEN_IDENTIFIER)
        goto houston;

      /* top level fields */
      if (!strcmp(t.sb, "patch"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_PATCH;
      else if (!strcmp(t.sb, "sequencer"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_SEQUENCER;
      /* patch fields */
      else if (!strcmp(t.sb, "program"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_PROGRAM;
      else if (!strcmp(t.sb, "osc_1"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_OSC_1;
      else if (!strcmp(t.sb, "osc_2"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_OSC_2;
      else if (!strcmp(t.sb, "osc_3"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_OSC_3;
      else if (!strcmp(t.sb, "osc_4"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_OSC_4;
      else if (!strcmp(t.sb, "feedback"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_FEEDBACK;
      else if (!strcmp(t.sb, "noise_period"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOISE_PERIOD;
      else if (!strcmp(t.sb, "noise_amplitude"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOISE_AMPLITUDE;
      else if (!strcmp(t.sb, "noise_alternate"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOISE_ALTERNATE;
      else if (!strcmp(t.sb, "carrier_envelope"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE;
      else if (!strcmp(t.sb, "modulator_envelope"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE;
      else if (!strcmp(t.sb, "extra_mode"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_EXTRA_MODE;
      else if (!strcmp(t.sb, "extra_rate"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_EXTRA_RATE;
      else if (!strcmp(t.sb, "extra_keyscaling"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_EXTRA_KEYSCALING;
      else if (!strcmp(t.sb, "vibrato_mode"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_VIBRATO_MODE;
      else if (!strcmp(t.sb, "vibrato_alternate"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_VIBRATO_ALTERNATE;
      else if (!strcmp(t.sb, "tremolo_mode"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_TREMOLO_MODE;
      else if (!strcmp(t.sb, "wobble_mode"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_WOBBLE_MODE;
      else if (!strcmp(t.sb, "lowpass"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_LOWPASS;
      else if (!strcmp(t.sb, "highpass"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_HIGHPASS;
      /* oscillator fields */
      else if (!strcmp(t.sb, "numerator"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NUMERATOR;
      else if (!strcmp(t.sb, "denominator"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_DENOMINATOR;
      else if (!strcmp(t.sb, "detune"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_DETUNE;
      else if (!strcmp(t.sb, "amplitude"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_AMPLITUDE;
      /* envelope fields */
      else if (!strcmp(t.sb, "attack"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_ATTACK;
      else if (!strcmp(t.sb, "decay_1"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_DECAY_1;
      else if (!strcmp(t.sb, "decay_2"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_DECAY_2;
      else if (!strcmp(t.sb, "release"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_RELEASE;
      else if (!strcmp(t.sb, "sustain"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_SUSTAIN;
      else if (!strcmp(t.sb, "rate_keyscaling"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_RATE_KEYSCALING;
      else if (!strcmp(t.sb, "level_keyscaling"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_LEVEL_KEYSCALING;
      /* sequencer fields */
      else if (!strcmp(t.sb, "key_signature"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_KEY_SIGNATURE;
      else if (!strcmp(t.sb, "time_signature"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_TIME_SIGNATURE;
      else if (!strcmp(t.sb, "tempo"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_TEMPO;
      else if (!strcmp(t.sb, "swing"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_SWING;
      else if (!strcmp(t.sb, "pattern"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_PATTERN;
      /* pattern fields */
      else if (!strcmp(t.sb, "number_of_bars"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NUMBER_OF_BARS;
      else if (!strcmp(t.sb, "step"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_STEP;
      /* step fields */
      else if (!strcmp(t.sb, "note_1"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOTE_1;
      else if (!strcmp(t.sb, "note_2"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOTE_2;
      else if (!strcmp(t.sb, "note_3"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOTE_3;
      else if (!strcmp(t.sb, "note_4"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_NOTE_4;
      else if (!strcmp(t.sb, "modulation"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_MODULATION;
      else if (!strcmp(t.sb, "volume"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_VOLUME;
      else if (!strcmp(t.sb, "brightness"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_BRIGHTNESS;
      else if (!strcmp(t.sb, "arpeggio"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO;
      else if (!strcmp(t.sb, "pitch_sweep"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP;
      else if (!strcmp(t.sb, "vibrato"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_VIBRATO;
      else if (!strcmp(t.sb, "tremolo"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_TREMOLO;
      else if (!strcmp(t.sb, "wobble"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_WOBBLE;
      /* arpeggio, lfo, and sweep fields */
      else if (!strcmp(t.sb, "mode"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_MODE;
      else if (!strcmp(t.sb, "depth"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_DEPTH;
      else if (!strcmp(t.sb, "speed"))
        current->type = DATA_TREE_NODE_TYPE_FIELD_SPEED;
      else
        goto houston;

      DATA_TREE_PUSH_NODE(stack, current)
      tokenizer_advance(&t);
      parse_state = PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE;
    }
    /* integer */
    else if ( (t.token == TOKEN_NUMBER_INTEGER) &&
              (parse_state == PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE))
    {
      DATA_TREE_CREATE_NEW_NODE(stack, current)
      current->type = DATA_TREE_NODE_TYPE_VALUE_INTEGER;
      current->value = strdup(t.sb);
      tokenizer_advance(&t);
      parse_state = PARSE_STATE_END_OF_FIELD;
    }
    /* string */
    else if ( (t.token == TOKEN_STRING) &&
              (parse_state == PARSE_STATE_ATTRIBUTE_SUBFIELD_OR_VALUE))

    {
      DATA_TREE_CREATE_NEW_NODE(stack, current)
      current->type = DATA_TREE_NODE_TYPE_VALUE_STRING;
      current->value = strdup(t.sb);
      tokenizer_advance(&t);
      parse_state = PARSE_STATE_END_OF_FIELD;
    }
    /* end of field */
    else if ( (t.token == TOKEN_GREATER_THAN) &&
              ( (parse_state == PARSE_STATE_END_OF_FIELD) ||
                (parse_state == PARSE_STATE_SUBFIELD_OR_END_OF_FIELD)))
    {
      current = stack[stack_top];
      DATA_TREE_POP_NODE(stack)
      tokenizer_advance(&t);
      parse_state = PARSE_STATE_SUBFIELD_OR_END_OF_FIELD;
    }
    /* error */
    else
      goto houston;
  }

  /* read eof and cleanup */
  PARSE_EAT_TOKEN(TOKEN_EOF)

  goto cleanup;

  /* error handling */
houston:
  if (root != NULL)
  {
    data_tree_node_destroy_tree(root);
    root = NULL;
  }

  printf("Failed text file parsing on line number %d.\n", t.ln);

  /* cleanup */
cleanup:
  if (stack != NULL)
  {
    free(stack);
    stack = NULL;
  }

  tokenizer_close_file(&t);
  tokenizer_deinit(&t);

  return root;
}

/*******************************************************************************
** parse_data_tree_semantic_analysis()
*******************************************************************************/
short int parse_data_tree_semantic_analysis(int current_type, int parent_type)
{
  if ((current_type == DATA_TREE_NODE_TYPE_FIELD_GERSEMI) &&
      (parent_type != DATA_TREE_NODE_TYPE_NONE))
    return 1;
  /* top level fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_PATCH) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_SEQUENCER) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  /* patch fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_PROGRAM) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_OSC_1) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_OSC_2) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_OSC_3) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_OSC_4) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_FEEDBACK) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_PERIOD) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_AMPLITUDE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_ALTERNATE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_MODE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_RATE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_KEYSCALING) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO_MODE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO_ALTERNATE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_TREMOLO_MODE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_WOBBLE_MODE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_LOWPASS) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_HIGHPASS) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATCH))
    return 1;
  /* oscillator fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NUMERATOR) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_1)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_2)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_3)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_4))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_DENOMINATOR) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_1)        &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_2)        &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_3)        &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_4))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_DETUNE)  &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_1)    &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_2)    &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_3)    &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_4))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_AMPLITUDE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_1)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_2)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_3)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_OSC_4))
    return 1;
  /* envelope fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_ATTACK)          &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_DECAY_1)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_DECAY_2)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_RELEASE)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_SUSTAIN)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_RATE_KEYSCALING) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_LEVEL_KEYSCALING)  &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)   &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE))
    return 1;
  /* sequencer fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_KEY_SIGNATURE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SEQUENCER))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_TIME_SIGNATURE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SEQUENCER))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_TEMPO) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SEQUENCER))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_SWING) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SEQUENCER))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_PATTERN) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SEQUENCER))
    return 1;
  /* pattern fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NUMBER_OF_BARS) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATTERN))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_STEP) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PATTERN))
    return 1;
  /* step fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_1) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_2) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_3) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_4) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_MODULATION) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_VOLUME) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_BRIGHTNESS) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_TREMOLO) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_WOBBLE) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_STEP))
    return 1;
  /* arpeggio, lfo, and sweep fields */
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_MODE)    &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_DEPTH)   &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_VIBRATO)  &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_TREMOLO)  &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_WOBBLE))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_FIELD_SPEED)       &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO)     &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP)  &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_VIBRATO)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_TREMOLO)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_WOBBLE))
    return 1;
  /* attributes */
  else if ( (current_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_SAMPLING) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_BITRES) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_SYSTEM) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_FORK) &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI))
    return 1;
  /* values */
  else if ( (current_type == DATA_TREE_NODE_TYPE_VALUE_INTEGER)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_FEEDBACK)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOISE_PERIOD)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOISE_AMPLITUDE)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOISE_ALTERNATE)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_EXTRA_MODE)           &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_EXTRA_RATE)           &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_EXTRA_KEYSCALING)     &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_VIBRATO_MODE)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_VIBRATO_ALTERNATE)    &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_TREMOLO_MODE)         &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_WOBBLE_MODE)          &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_LOWPASS)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_HIGHPASS)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NUMERATOR)            &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_DENOMINATOR)          &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_DETUNE)               &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_AMPLITUDE)            &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_ATTACK)               &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_DECAY_1)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_DECAY_2)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_RELEASE)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SUSTAIN)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_RATE_KEYSCALING)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_LEVEL_KEYSCALING)     &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_TEMPO)                &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NUMBER_OF_BARS)       &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODULATION)           &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_VOLUME)               &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_BRIGHTNESS)           &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_MODE)                 &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_DEPTH)                &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SPEED)                &&
            (parent_type != DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_SAMPLING)  &&
            (parent_type != DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_BITRES))
    return 1;
  else if ( (current_type == DATA_TREE_NODE_TYPE_VALUE_STRING)            &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_PROGRAM)            &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_KEY_SIGNATURE)      &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_TIME_SIGNATURE)     &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_SWING)              &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOTE_1)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOTE_2)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOTE_3)             &&
            (parent_type != DATA_TREE_NODE_TYPE_FIELD_NOTE_4)             &&
            (parent_type != DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_SYSTEM)  &&
            (parent_type != DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_FORK))
    return 1;

  return 0;
}

/*******************************************************************************
** parse_data_tree_load_integer()
*******************************************************************************/
short int parse_data_tree_load_integer( int val, 
                                        int parent_type, 
                                        int grand_type, 
                                        int great_type)
{
  patch*    pc;

  pattern*  pt;
  step*     st;

  pc = &G_synth_patch_bank[0];

  pt = &G_sequencer_patterns[G_sequencer_num_patterns - 1];
  st = &pt->steps[pt->num_steps - 1];

  /* feedback */
  if (parent_type == DATA_TREE_NODE_TYPE_FIELD_FEEDBACK)
  {
    if ((val >= 0) && (val <= 8))
      pc->feedback = val;
    else
    {
      printf("Invalid Feedback specified. Defaulting to 0.\n");
      pc->feedback = 0;
    }
  }
  /* noise period */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_PERIOD)
  {
    if ((val >= 0) && (val <= 15))
      pc->noise_period = val;
    else
    {
      printf("Invalid Noise Period specified. Defaulting to 0.\n");
      pc->noise_period = 0;
    }
  }
  /* noise amplitude */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_AMPLITUDE)
  {
    if ((val >= 0) && (val <= 16))
      pc->noise_amplitude = val;
    else
    {
      printf("Invalid Noise Amplitude specified. Defaulting to 0.\n");
      pc->noise_amplitude = 0;
    }
  }
  /* noise alternate flag */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOISE_ALTERNATE)
  {
    if ((val >= 0) && (val <= 1))
      pc->noise_alternate = val;
    else
    {
      printf("Invalid Noise Alternate Flag specified. Defaulting to 0.\n");
      pc->noise_alternate = 0;
    }
  }
  /* extra mode */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_MODE)
  {
    if ((val >= 0) && (val <= 9))
      pc->extra_mode = val;
    else
    {
      printf("Invalid Extra Mode specified. Defaulting to OFF.\n");
      pc->extra_mode = LINEAR_MODE_CONSTANT;
    }
  }
  /* extra rate */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_RATE)
  {
    if ((val >= 1) && (val <= 32))
      pc->extra_rate = val;
    else
    {
      printf("Invalid Extra Rate specified. Defaulting to 1.\n");
      pc->extra_rate = 1;
    }
  }
  /* extra keyscaling */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_EXTRA_KEYSCALING)
  {
    if ((val >= 1) && (val <= 8))
      pc->extra_keyscaling = val;
    else
    {
      printf("Invalid Extra Keyscaling specified. Defaulting to 1.\n");
      pc->extra_keyscaling = 1;
    }
  }
  /* vibrato mode */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO_MODE)
  {
    if ((val >= 0) && (val <= 9))
      pc->vibrato_mode = val;
    else
    {
      printf("Invalid Vibrato Mode specified. Defaulting to OFF.\n");
      pc->vibrato_mode = LFO_MODE_TRIANGLE;
    }
  }
  /* vibrato alternate flag */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO_ALTERNATE)
  {
    if ((val >= 0) && (val <= 1))
      pc->vibrato_alternate = val;
    else
    {
      printf("Invalid Vibrato Alternate Flag specified. Defaulting to 0.\n");
      pc->vibrato_alternate = 0;
    }
  }
  /* tremolo mode */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_TREMOLO_MODE)
  {
    if ((val >= 0) && (val <= 9))
      pc->tremolo_mode = val;
    else
    {
      printf("Invalid Tremolo Mode specified. Defaulting to OFF.\n");
      pc->tremolo_mode = LFO_MODE_TRIANGLE;
    }
  }
  /* wobble mode */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_WOBBLE_MODE)
  {
    if ((val >= 0) && (val <= 9))
      pc->wobble_mode = val;
    else
    {
      printf("Invalid Wobble Mode specified. Defaulting to OFF.\n");
      pc->wobble_mode = LFO_MODE_TRIANGLE;
    }
  }
  /* lowpass */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_LOWPASS)
  {
    if ((val >= 0) && (val <= 3))
      pc->lowpass_cutoff = val;
    else
    {
      printf("Invalid Lowpass Filter Cutoff specified. Defaulting to 3.\n");
      pc->lowpass_cutoff = 3;
    }
  }
  /* highpass */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_HIGHPASS)
  {
    if ((val >= 0) && (val <= 3))
      pc->highpass_cutoff = val;
    else
    {
      printf("Invalid Highpass Filter Cutoff specified. Defaulting to 0.\n");
      pc->highpass_cutoff = 0;
    }
  }
  /* multiple (numerator) */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NUMERATOR)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_1)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_1_numerator = val;
      else
      {
        printf("Invalid Osc 1 Numerator specified. Defaulting to 1.\n");
        pc->osc_1_numerator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_2)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_2_numerator = val;
      else
      {
        printf("Invalid Osc 2 Numerator specified. Defaulting to 1.\n");
        pc->osc_2_numerator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_3)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_3_numerator = val;
      else
      {
        printf("Invalid Osc 3 Numerator specified. Defaulting to 1.\n");
        pc->osc_3_numerator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_4)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_4_numerator = val;
      else
      {
        printf("Invalid Osc 4 Numerator specified. Defaulting to 1.\n");
        pc->osc_4_numerator = 1;
      }
    }
  }
  /* multiple (denominator) */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_DENOMINATOR)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_1)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_1_denominator = val;
      else
      {
        printf("Invalid Osc 1 Denominator specified. Defaulting to 1.\n");
        pc->osc_1_denominator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_2)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_2_denominator = val;
      else
      {
        printf("Invalid Osc 2 Denominator specified. Defaulting to 1.\n");
        pc->osc_2_denominator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_3)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_3_denominator = val;
      else
      {
        printf("Invalid Osc 3 Denominator specified. Defaulting to 1.\n");
        pc->osc_3_denominator = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_4)
    {
      if ((val >= 1) && (val <= 16))
        pc->osc_4_denominator = val;
      else
      {
        printf("Invalid Osc 4 Denominator specified. Defaulting to 1.\n");
        pc->osc_4_denominator = 1;
      }
    }
  }
  /* detune */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_DETUNE)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_1)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_1_detune = val;
      else
      {
        printf("Invalid Osc 1 Detune specified. Defaulting to 8.\n");
        pc->osc_1_detune = 8;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_2)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_2_detune = val;
      else
      {
        printf("Invalid Osc 2 Detune specified. Defaulting to 8.\n");
        pc->osc_2_detune = 8;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_3)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_3_detune = val;
      else
      {
        printf("Invalid Osc 3 Detune specified. Defaulting to 8.\n");
        pc->osc_3_detune = 8;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_4)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_4_detune = val;
      else
      {
        printf("Invalid Osc 4 Detune specified. Defaulting to 8.\n");
        pc->osc_4_detune = 8;
      }
    }
  }
  /* amplitude */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_AMPLITUDE)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_1)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_1_amplitude = val;
      else
      {
        printf("Invalid Osc 1 Amplitude specified. Defaulting to 16.\n");
        pc->osc_1_amplitude = 16;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_2)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_2_amplitude = val;
      else
      {
        printf("Invalid Osc 2 Amplitude specified. Defaulting to 16.\n");
        pc->osc_2_amplitude = 16;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_3)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_3_amplitude = val;
      else
      {
        printf("Invalid Osc 3 Amplitude specified. Defaulting to 16.\n");
        pc->osc_3_amplitude = 16;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_OSC_4)
    {
      if ((val >= 0) && (val <= 16))
        pc->osc_4_amplitude = val;
      else
      {
        printf("Invalid Osc 4 Amplitude specified. Defaulting to 16.\n");
        pc->osc_4_amplitude = 16;
      }
    }
  }
  /* attack */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_ATTACK)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->carr_attack = val;
      else
      {
        printf("Invalid Carrier Envelope Attack specified. Defaulting to 1.\n");
        pc->carr_attack = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->mod_attack = val;
      else
      {
        printf("Invalid Modulator Envelope Attack specified. Defaulting to 1.\n");
        pc->mod_attack = 1;
      }
    }
  }
  /* decay 1 */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_DECAY_1)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->carr_decay_1 = val;
      else
      {
        printf("Invalid Carrier Envelope Decay 1 specified. Defaulting to 1.\n");
        pc->carr_decay_1 = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->mod_decay_1 = val;
      else
      {
        printf("Invalid Modulator Envelope Decay 1 specified. Defaulting to 1.\n");
        pc->mod_decay_1 = 1;
      }
    }
  }
  /* decay 2 */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_DECAY_2)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->carr_decay_2 = val;
      else
      {
        printf("Invalid Carrier Envelope Decay 2 specified. Defaulting to 1.\n");
        pc->carr_decay_2 = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->mod_decay_2 = val;
      else
      {
        printf("Invalid Modulator Envelope Decay 2 specified. Defaulting to 1.\n");
        pc->mod_decay_2 = 1;
      }
    }
  }
  /* release */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_RELEASE)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->carr_release = val;
      else
      {
        printf("Invalid Carrier Envelope Release specified. Defaulting to 1.\n");
        pc->carr_release = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 32))
        pc->mod_release = val;
      else
      {
        printf("Invalid Modulator Envelope Release specified. Defaulting to 1.\n");
        pc->mod_release = 1;
      }
    }
  }
  /* sustain */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_SUSTAIN)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 0) && (val <= 16))
        pc->carr_sustain = val;
      else
      {
        printf("Invalid Carrier Envelope Sustain specified. Defaulting to 16.\n");
        pc->carr_sustain = 16;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 0) && (val <= 16))
        pc->mod_sustain = val;
      else
      {
        printf("Invalid Modulator Envelope Sustain specified. Defaulting to 16.\n");
        pc->mod_sustain = 16;
      }
    }
  }
  /* rate keyscaling */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_RATE_KEYSCALING)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 8))
        pc->carr_rate_keyscaling = val;
      else
      {
        printf("Invalid Carrier Envelope Rate Keyscaling specified. Defaulting to 1.\n");
        pc->carr_rate_keyscaling = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 8))
        pc->mod_rate_keyscaling = val;
      else
      {
        printf("Invalid Modulator Envelope Rate Keyscaling specified. Defaulting to 1.\n");
        pc->mod_rate_keyscaling = 1;
      }
    }
  }
  /* level keyscaling */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_LEVEL_KEYSCALING)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE)
    {
      if ((val >= 1) && (val <= 8))
        pc->carr_level_keyscaling = val;
      else
      {
        printf("Invalid Carrier Envelope Level Keyscaling specified. Defaulting to 1.\n");
        pc->carr_level_keyscaling = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE)
    {
      if ((val >= 1) && (val <= 8))
        pc->mod_level_keyscaling = val;
      else
      {
        printf("Invalid Modulator Envelope Level Keyscaling specified. Defaulting to 1.\n");
        pc->mod_level_keyscaling = 1;
      }
    }
  }
  /* tempo */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_TEMPO)
  {
    if ((val >= TEMPO_LOWER_BOUND) && (val <= TEMPO_UPPER_BOUND))
      sequencer_set_tempo(val);
    else
    {
      printf("Invalid Tempo specified. Defaulting to 120.\n");
      sequencer_set_tempo(120);
    }
  }
  /* number of bars */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NUMBER_OF_BARS)
  {
    if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_4_4)
    {
      if ((val >= 1) && (val <= SEQUENCER_MAX_STEPS_PER_PATTERN / (4 * 4)))
        pt->number_of_bars = val;
      else
      {
        printf("Invalid Number of Bars specified. Defaulting to 4.\n");
        pt->number_of_bars = 4;
      }
    }
    else if (G_sequencer_time_signature == SEQUENCER_TIME_SIGNATURE_3_8)
    {
      if ((val >= 1) && (val <= SEQUENCER_MAX_STEPS_PER_PATTERN / (3 * 4)))
        pt->number_of_bars = val;
      else
      {
        printf("Invalid Number of Bars specified. Defaulting to 4.\n");
        pt->number_of_bars = 4;
      }
    }
  }
  /* modulation */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_MODULATION)
  {
    if ((val >= 0) && (val <= 15))
      st->modulation = val;
    else
    {
      printf("Invalid Step Modulation (Key Change) specified. Defaulting to 0.\n");
      st->modulation = 0;
    }
  }
  /* volume */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_VOLUME)
  {
    if ((val >= 1) && (val <= 9))
      st->volume = val;
    else
    {
      printf("Invalid Step Volume specified. Defaulting to 5.\n");
      st->volume = 5;
    }
  }
  /* brightness */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_BRIGHTNESS)
  {
    if ((val >= 1) && (val <= 9))
      st->brightness = val;
    else
    {
      printf("Invalid Step Brightness specified. Defaulting to 5.\n");
      st->brightness = 5;
    }
  }
  /* mode */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_MODE)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO)
    {
      if ((val >= 0) && (val <= 9))
        st->arpeggio_mode = val;
      else
      {
        printf("Invalid Step Arpeggio Mode specified. Defaulting to 0.\n");
        st->arpeggio_mode = 0;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP)
    {
      if ((val >= 0) && (val <= 9))
        st->pitch_sweep_mode = val;
      else
      {
        printf("Invalid Step Pitch Sweep Mode specified. Defaulting to 0.\n");
        st->pitch_sweep_mode = 0;
      }
    }
  }
  /* depth */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_DEPTH)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO)
    {
      if ((val >= 0) && (val <= 9))
        st->vibrato_depth = val;
      else
      {
        printf("Invalid Step Vibrato Depth specified. Defaulting to 0.\n");
        st->vibrato_depth = 0;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_TREMOLO)
    {
      if ((val >= 0) && (val <= 9))
        st->tremolo_depth = val;
      else
      {
        printf("Invalid Step Tremolo Depth specified. Defaulting to 0.\n");
        st->tremolo_depth = 0;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_WOBBLE)
    {
      if ((val >= 0) && (val <= 9))
        st->wobble_depth = val;
      else
      {
        printf("Invalid Step Wobble Depth specified. Defaulting to 0.\n");
        st->wobble_depth = 0;
      }
    }
  }
  /* speed */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_SPEED)
  {
    if (grand_type == DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO)
    {
      if ((val >= 1) && (val <= 9))
        st->arpeggio_speed = val;
      else
      {
        printf("Invalid Step Arpeggio Speed specified. Defaulting to 1.\n");
        st->arpeggio_speed = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP)
    {
      if ((val >= 1) && (val <= 9))
        st->pitch_sweep_speed = val;
      else
      {
        printf("Invalid Step Pitch Sweep Speed specified. Defaulting to 1.\n");
        st->pitch_sweep_speed = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_VIBRATO)
    {
      if ((val >= 1) && (val <= 9))
        st->vibrato_speed = val;
      else
      {
        printf("Invalid Step Vibrato Speed specified. Defaulting to 1.\n");
        st->vibrato_speed = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_TREMOLO)
    {
      if ((val >= 1) && (val <= 9))
        st->tremolo_speed = val;
      else
      {
        printf("Invalid Step Tremolo Speed specified. Defaulting to 1.\n");
        st->tremolo_speed = 1;
      }
    }
    else if (grand_type == DATA_TREE_NODE_TYPE_FIELD_WOBBLE)
    {
      if ((val >= 1) && (val <= 9))
        st->wobble_speed = val;
      else
      {
        printf("Invalid Step Wobble Speed specified. Defaulting to 1.\n");
        st->wobble_speed = 1;
      }
    }
  }
  /* export sampling rate */
  else if (parent_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_SAMPLING)
  {
    if (val == 22050)
      export_set_sample_rate(EXPORT_SAMPLE_RATE_22050);
    else if (val == 36000)
      export_set_sample_rate(EXPORT_SAMPLE_RATE_36000);
    else
    {
      printf("Invalid export sampling rate specified. Defaulting to 22050 hz.\n");

      export_set_sample_rate(EXPORT_SAMPLE_RATE_22050);
    }
  }
  /* export bit resolution */
  else if (parent_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_BITRES)
  {
    if (val == 8)
      export_set_bit_resolution(EXPORT_BIT_RESOLUTION_08);
    else if (val == 16)
      export_set_bit_resolution(EXPORT_BIT_RESOLUTION_16);
    else
    {
      printf("Invalid export bitres specified. Defaulting to 16 bit.\n");

      export_set_bit_resolution(EXPORT_BIT_RESOLUTION_16);
    }
  }

  return 0;
}

/*******************************************************************************
** parse_data_tree_load_string()
*******************************************************************************/
short int parse_data_tree_load_string(char* name, 
                                      int parent_type, 
                                      int grand_type, 
                                      int great_type)
{
  patch*    pc;

  pattern*  pt;
  step*     st;

  int       note_val;

  if (name == NULL)
    return 1;

  pc = &G_synth_patch_bank[0];

  pt = &G_sequencer_patterns[G_sequencer_num_patterns - 1];
  st = &pt->steps[pt->num_steps - 1];

  /* program */
  if (parent_type == DATA_TREE_NODE_TYPE_FIELD_PROGRAM)
  {
    if (!strcmp(name, "sync_square"))
      pc->program = VOICE_PROGRAM_SYNC_SQUARE;
    else if (!strcmp(name, "sync_triangle"))
      pc->program = VOICE_PROGRAM_SYNC_TRIANGLE;
    else if (!strcmp(name, "sync_saw"))
      pc->program = VOICE_PROGRAM_SYNC_SAW;
    else if (!strcmp(name, "sync_phat_saw"))
      pc->program = VOICE_PROGRAM_SYNC_PHAT_SAW;
    else if (!strcmp(name, "ring_square"))
      pc->program = VOICE_PROGRAM_RING_SQUARE;
    else if (!strcmp(name, "ring_triangle"))
      pc->program = VOICE_PROGRAM_RING_TRIANGLE;
    else if (!strcmp(name, "ring_saw"))
      pc->program = VOICE_PROGRAM_RING_SAW;
    else if (!strcmp(name, "ring_phat_saw"))
      pc->program = VOICE_PROGRAM_RING_PHAT_SAW;
    else if (!strcmp(name, "pulse_waves"))
      pc->program = VOICE_PROGRAM_PULSE_WAVES;
    else if (!strcmp(name, "fm_1_carrier_chain"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_CHAIN;
    else if (!strcmp(name, "fm_1_carrier_y"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_Y;
    else if (!strcmp(name, "fm_1_carrier_left_crab_claw"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_LEFT_CRAB_CLAW;
    else if (!strcmp(name, "fm_1_carrier_right_crab_claw"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_RIGHT_CRAB_CLAW;
    else if (!strcmp(name, "fm_1_carrier_diamond"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_DIAMOND;
    else if (!strcmp(name, "fm_1_carrier_three_to_one"))
      pc->program = VOICE_PROGRAM_FM_1_CARRIER_THREE_TO_ONE;
    else if (!strcmp(name, "fm_2_carriers_twin"))
      pc->program = VOICE_PROGRAM_FM_2_CARRIERS_TWIN;
    else if (!strcmp(name, "fm_2_carriers_stack"))
      pc->program = VOICE_PROGRAM_FM_2_CARRIERS_STACK;
    else if (!strcmp(name, "fm_2_carriers_stack_alt"))
      pc->program = VOICE_PROGRAM_FM_2_CARRIERS_STACK_ALT;
    else if (!strcmp(name, "fm_2_carriers_shared"))
      pc->program = VOICE_PROGRAM_FM_2_CARRIERS_SHARED;
    else if (!strcmp(name, "fm_3_carriers_one_to_three"))
      pc->program = VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_THREE;
    else if (!strcmp(name, "fm_3_carriers_one_to_two"))
      pc->program = VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_TWO;
    else if (!strcmp(name, "fm_3_carriers_one_to_one"))
      pc->program = VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE;
    else if (!strcmp(name, "fm_3_carriers_one_to_one_alt"))
      pc->program = VOICE_PROGRAM_FM_3_CARRIERS_ONE_TO_ONE_ALT;
    else if (!strcmp(name, "fm_4_carriers_pipes"))
      pc->program = VOICE_PROGRAM_FM_4_CARRIERS_PIPES;
    else
    {
      printf("Invalid Voice Program specified. Defaulting to Sync Square.\n");
      pc->program = VOICE_PROGRAM_SYNC_SQUARE;
    }
  }
  /* key signature */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_KEY_SIGNATURE)
  {
    if (!strcmp(name, "c_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_FLAT_MAJOR);
    else if (!strcmp(name, "g_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_G_FLAT_MAJOR);
    else if (!strcmp(name, "d_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_D_FLAT_MAJOR);
    else if (!strcmp(name, "a_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_A_FLAT_MAJOR);
    else if (!strcmp(name, "e_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_E_FLAT_MAJOR);
    else if (!strcmp(name, "b_flat_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_B_FLAT_MAJOR);
    else if (!strcmp(name, "f_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_F_MAJOR);
    else if (!strcmp(name, "c_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_MAJOR);
    else if (!strcmp(name, "g_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_G_MAJOR);
    else if (!strcmp(name, "d_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_D_MAJOR);
    else if (!strcmp(name, "a_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_A_MAJOR);
    else if (!strcmp(name, "e_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_E_MAJOR);
    else if (!strcmp(name, "b_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_B_MAJOR);
    else if (!strcmp(name, "f_sharp_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_F_SHARP_MAJOR);
    else if (!strcmp(name, "c_sharp_major"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_SHARP_MAJOR);
    else if (!strcmp(name, "a_flat_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_A_FLAT_MINOR);
    else if (!strcmp(name, "e_flat_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_E_FLAT_MINOR);
    else if (!strcmp(name, "b_flat_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_B_FLAT_MINOR);
    else if (!strcmp(name, "f_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_F_MINOR);
    else if (!strcmp(name, "c_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_MINOR);
    else if (!strcmp(name, "g_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_G_MINOR);
    else if (!strcmp(name, "d_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_D_MINOR);
    else if (!strcmp(name, "a_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_A_MINOR);
    else if (!strcmp(name, "e_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_E_MINOR);
    else if (!strcmp(name, "b_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_B_MINOR);
    else if (!strcmp(name, "f_sharp_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_F_SHARP_MINOR);
    else if (!strcmp(name, "c_sharp_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_SHARP_MINOR);
    else if (!strcmp(name, "g_sharp_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_G_SHARP_MINOR);
    else if (!strcmp(name, "d_sharp_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_D_SHARP_MINOR);
    else if (!strcmp(name, "a_sharp_minor"))
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_A_SHARP_MINOR);
    else
    {
      printf("Invalid Key Signature specified. Defaulting to C Major.\n");
      sequencer_set_key_signature(SEQUENCER_KEY_SIGNATURE_C_MAJOR);
    }
  }
  /* time signature */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_TIME_SIGNATURE)
  {
    if (!strcmp(name, "4_4"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_4_4);
    else if (!strcmp(name, "3_4"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_3_4);
    else if (!strcmp(name, "2_4"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_2_4);
    else if (!strcmp(name, "2_2"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_2_2);
    else if (!strcmp(name, "3_8"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_3_8);
    else if (!strcmp(name, "6_8"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_6_8);
    else if (!strcmp(name, "9_8"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_9_8);
    else if (!strcmp(name, "12_8"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_12_8);
    else if (!strcmp(name, "5_4"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_5_4);
    else if (!strcmp(name, "6_4"))
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_6_4);
    else
    {
      printf("Invalid Time Signature specified. Defaulting to 4/4.\n");
      sequencer_set_time_signature(SEQUENCER_TIME_SIGNATURE_4_4);
    }
  }
  /* swing */
  else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_SWING)
  {
    if (!strcmp(name, "1_1"))
      sequencer_set_swing(TEMPO_SWING_1_1);
    else if (!strcmp(name, "5_4"))
      sequencer_set_swing(TEMPO_SWING_5_4);
    else if (!strcmp(name, "4_3"))
      sequencer_set_swing(TEMPO_SWING_4_3);
    else if (!strcmp(name, "3_2"))
      sequencer_set_swing(TEMPO_SWING_3_2);
    else if (!strcmp(name, "5_3"))
      sequencer_set_swing(TEMPO_SWING_5_3);
    else if (!strcmp(name, "2_1"))
      sequencer_set_swing(TEMPO_SWING_2_1);
    else if (!strcmp(name, "5_2"))
      sequencer_set_swing(TEMPO_SWING_5_2);
    else if (!strcmp(name, "3_1"))
      sequencer_set_swing(TEMPO_SWING_3_1);
    else
    {
      printf("Invalid Swing specified. Defaulting to 1:1.\n");
      sequencer_set_swing(TEMPO_SWING_1_1);
    }
  }
  /* note */
  else if ( (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_1) || 
            (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_2) || 
            (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_3) || 
            (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_4))
  {
    if (!strcmp(name, "1_1"))
      note_val = 1;
    else if (!strcmp(name, "1_2"))
      note_val = 2;
    else if (!strcmp(name, "1_3"))
      note_val = 3;
    else if (!strcmp(name, "1_4"))
      note_val = 4;
    else if (!strcmp(name, "1_5"))
      note_val = 5;
    else if (!strcmp(name, "1_6"))
      note_val = 6;
    else if (!strcmp(name, "1_7"))
      note_val = 7;
    else if (!strcmp(name, "2_1"))
      note_val = 8;
    else if (!strcmp(name, "2_2"))
      note_val = 9;
    else if (!strcmp(name, "2_3"))
      note_val = 10;
    else if (!strcmp(name, "2_4"))
      note_val = 11;
    else if (!strcmp(name, "2_5"))
      note_val = 12;
    else if (!strcmp(name, "2_6"))
      note_val = 13;
    else if (!strcmp(name, "2_7"))
      note_val = 14;
    else if (!strcmp(name, "3_1"))
      note_val = 15;
    else if (!strcmp(name, "3_2"))
      note_val = 16;
    else if (!strcmp(name, "3_3"))
      note_val = 17;
    else if (!strcmp(name, "3_4"))
      note_val = 18;
    else if (!strcmp(name, "3_5"))
      note_val = 19;
    else if (!strcmp(name, "3_6"))
      note_val = 20;
    else if (!strcmp(name, "3_7"))
      note_val = 21;
    else if (!strcmp(name, "4_1"))
      note_val = 22;
    else if (!strcmp(name, "4_2"))
      note_val = 23;
    else if (!strcmp(name, "4_3"))
      note_val = 24;
    else if (!strcmp(name, "4_4"))
      note_val = 25;
    else if (!strcmp(name, "4_5"))
      note_val = 26;
    else if (!strcmp(name, "4_6"))
      note_val = 27;
    else if (!strcmp(name, "4_7"))
      note_val = 28;
    else if (!strcmp(name, "5_1"))
      note_val = 29;
    else if (!strcmp(name, "5_2"))
      note_val = 30;
    else if (!strcmp(name, "5_3"))
      note_val = 31;
    else if (!strcmp(name, "5_4"))
      note_val = 32;
    else if (!strcmp(name, "5_5"))
      note_val = 33;
    else if (!strcmp(name, "5_6"))
      note_val = 34;
    else if (!strcmp(name, "5_7"))
      note_val = 35;
    else if (!strcmp(name, "5_8"))
      note_val = 36;
    else if (!strcmp(name, "rest"))
      note_val = 37;
    else if (!strcmp(name, "blank"))
      note_val = 0;
    else
    {
      printf("Invalid Note specified. Defaulting to Blank.\n");
      note_val = 0;
    }

    if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_1)
      st->note_1 = note_val;
    else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_2)
      st->note_2 = note_val;
    else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_3)
      st->note_3 = note_val;
    else if (parent_type == DATA_TREE_NODE_TYPE_FIELD_NOTE_4)
      st->note_4 = note_val;
  }
  /* tuning system */
  else if (parent_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_SYSTEM)
  {
    if (!strcmp(name, "equal_temperament"))
      tuning_set_system(TUNING_SYSTEM_12_EQUAL_TEMPERAMENT);
    else if (!strcmp(name, "pythagorean"))
      tuning_set_system(TUNING_SYSTEM_PYTHAGOREAN);
    else if (!strcmp(name, "quarter_comma_meantone"))
      tuning_set_system(TUNING_SYSTEM_QUARTER_COMMA_MEANTONE);
    else if (!strcmp(name, "werckmeister_iii"))
      tuning_set_system(TUNING_SYSTEM_WERCKMEISTER_III);
    else
    {
      printf("Invalid tuning system specified. Defaulting to Equal Temperament.\n");
      tuning_set_system(TUNING_SYSTEM_12_EQUAL_TEMPERAMENT);
    }
  }
  /* tuning fork */
  else if (parent_type == DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_FORK)
  {
    if (!strcmp(name, "c256"))
      tuning_set_fork(TUNING_FORK_C256);
    else if (!strcmp(name, "a432"))
      tuning_set_fork(TUNING_FORK_A432);
    else if (!strcmp(name, "a434"))
      tuning_set_fork(TUNING_FORK_A434);
    else if (!strcmp(name, "a436"))
      tuning_set_fork(TUNING_FORK_A436);
    else if (!strcmp(name, "a438"))
      tuning_set_fork(TUNING_FORK_A438);
    else if (!strcmp(name, "a440"))
      tuning_set_fork(TUNING_FORK_A440);
    else if (!strcmp(name, "a442"))
      tuning_set_fork(TUNING_FORK_A442);
    else if (!strcmp(name, "a444"))
      tuning_set_fork(TUNING_FORK_A444);
    else
    {
      printf("Invalid tuning fork specified. Defaulting to A440.\n");
      tuning_set_fork(TUNING_FORK_A440);
    }
  }

  return 0;
}

/*******************************************************************************
** parse_data_tree_to_globals()
*******************************************************************************/
short int parse_data_tree_to_globals(data_tree_node* root)
{
  data_tree_node*   current;
  int               current_type;
  int               parent_type;
  int               grand_type;
  int               great_type;
  data_tree_node**  stack;
  int               stack_size;
  int               stack_top;

  if (root == NULL)
    return 1;

  /* setup stack */
  stack = malloc(DATA_TREE_STACK_INITIAL_SIZE * sizeof(data_tree_node*));
  stack_size = DATA_TREE_STACK_INITIAL_SIZE;
  stack_top = -1;

  /* verify root node */
  if ((root->type != DATA_TREE_NODE_TYPE_FIELD_GERSEMI) ||
      (root->sibling != NULL)                           ||
      (root->child == NULL))
  {
    printf("Invalid root node.\n");
    goto houston;
  }

  /* push root node onto stack and begin traversing the subtree */
  DATA_TREE_PUSH_NODE(stack, root)
  current = root->child;

  while(stack_top >= 0)
  {
    current_type = current->type;
    parent_type = stack[stack_top]->type;

    if (stack_top == 0)
      grand_type = DATA_TREE_NODE_TYPE_NONE;
    else
      grand_type = stack[stack_top - 1]->type;

    if (stack_top <= 1)
      great_type = DATA_TREE_NODE_TYPE_NONE;
    else
      great_type = stack[stack_top - 2]->type;

    /* semantic analysis */
    if (parse_data_tree_semantic_analysis(current_type, parent_type))
    {
      printf("Semantic analysis failed.\n");
      goto houston;
    }

    /* process this node */
    if (current_type == DATA_TREE_NODE_TYPE_FIELD_PATTERN)
    {
      G_sequencer_num_patterns += 1;

      if (G_sequencer_num_patterns > SEQUENCER_MAX_PATTERNS)
      {
        printf("Too many sequencer patterns defined.\n");
        goto houston;
      }
    }
    else if (current_type == DATA_TREE_NODE_TYPE_FIELD_STEP)
    {
      G_sequencer_patterns[G_sequencer_num_patterns - 1].num_steps += 1;

      if (G_sequencer_patterns[G_sequencer_num_patterns - 1].num_steps > SEQUENCER_MAX_STEPS_PER_PATTERN)
      {
        printf("Too many pattern steps defined.\n");
        goto houston;
      }
    }
    else if (current_type == DATA_TREE_NODE_TYPE_VALUE_INTEGER)
    {
      parse_data_tree_load_integer( strtol(current->value, NULL, 10),
                                    parent_type, grand_type, great_type);
    }
    else if (current_type == DATA_TREE_NODE_TYPE_VALUE_STRING)
    {
      parse_data_tree_load_string(current->value, 
                                  parent_type, grand_type, great_type);
    }

    /* go to next node */
    if (current->child != NULL)
    {
      DATA_TREE_PUSH_NODE(stack, current)
      current = current->child;
    }
    else if (current->sibling != NULL)
    {
      current = current->sibling;
    }
    else
    {
      current = stack[stack_top];
      DATA_TREE_POP_NODE(stack)

      while ((stack_top >= 0) && (current->sibling == NULL))
      {
        current = stack[stack_top];
        DATA_TREE_POP_NODE(stack)
      }

      if (stack_top == -1)
        current = NULL;
      else
        current = current->sibling;
    }
  }

  goto cleanup;

  /* error handling */
houston:
  sequencer_reset();

  /* cleanup */
cleanup:
  if (stack != NULL)
  {
    free(stack);
    stack = NULL;
  }

  return 0;
}

