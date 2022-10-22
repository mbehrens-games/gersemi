/*******************************************************************************
** datatree.h (data tree node)
*******************************************************************************/

#ifndef DATA_TREE_H
#define DATA_TREE_H

#define DATA_TREE_STACK_INITIAL_SIZE  10

#define DATA_TREE_PUSH_NODE(stack, node)                                       \
  if (stack##_top >= stack##_size - 1)                                         \
  {                                                                            \
    stack = realloc(stack,                                                     \
                    (stack##_size + DATA_TREE_STACK_INITIAL_SIZE) *            \
                    sizeof(data_tree_node*));                                  \
  }                                                                            \
                                                                               \
  stack[++stack##_top] = node;

#define DATA_TREE_POP_NODE(stack)                                              \
  if (stack##_top >= 0)                                                        \
    stack##_top--;

#define DATA_TREE_CREATE_NEW_NODE(stack, current)                              \
  if (current == stack[stack##_top])                                           \
  {                                                                            \
    current->child = data_tree_node_create();                                  \
    current = current->child;                                                  \
  }                                                                            \
  else                                                                         \
  {                                                                            \
    current->sibling = data_tree_node_create();                                \
    current = current->sibling;                                                \
  }

enum
{
  DATA_TREE_NODE_TYPE_NONE,
  /* root fields */
  DATA_TREE_NODE_TYPE_FIELD_GERSEMI,
  /* top level fields */
  DATA_TREE_NODE_TYPE_FIELD_PATCH,
  DATA_TREE_NODE_TYPE_FIELD_SEQUENCER,
  /* patch fields */
  DATA_TREE_NODE_TYPE_FIELD_PROGRAM,
  DATA_TREE_NODE_TYPE_FIELD_OSC_1,
  DATA_TREE_NODE_TYPE_FIELD_OSC_2,
  DATA_TREE_NODE_TYPE_FIELD_OSC_3,
  DATA_TREE_NODE_TYPE_FIELD_OSC_4,
  DATA_TREE_NODE_TYPE_FIELD_FEEDBACK,
  DATA_TREE_NODE_TYPE_FIELD_NOISE_PERIOD,
  DATA_TREE_NODE_TYPE_FIELD_NOISE_AMPLITUDE,
  DATA_TREE_NODE_TYPE_FIELD_NOISE_ALTERNATE,
  DATA_TREE_NODE_TYPE_FIELD_CARRIER_ENVELOPE,
  DATA_TREE_NODE_TYPE_FIELD_MODULATOR_ENVELOPE,
  DATA_TREE_NODE_TYPE_FIELD_EXTRA_MODE,
  DATA_TREE_NODE_TYPE_FIELD_EXTRA_RATE,
  DATA_TREE_NODE_TYPE_FIELD_EXTRA_KEYSCALING,
  DATA_TREE_NODE_TYPE_FIELD_VIBRATO_MODE,
  DATA_TREE_NODE_TYPE_FIELD_VIBRATO_ALTERNATE,
  DATA_TREE_NODE_TYPE_FIELD_TREMOLO_MODE,
  DATA_TREE_NODE_TYPE_FIELD_WOBBLE_MODE,
  DATA_TREE_NODE_TYPE_FIELD_LOWPASS,
  DATA_TREE_NODE_TYPE_FIELD_HIGHPASS,
  /* oscillator fields */
  DATA_TREE_NODE_TYPE_FIELD_NUMERATOR,
  DATA_TREE_NODE_TYPE_FIELD_DENOMINATOR,
  DATA_TREE_NODE_TYPE_FIELD_DETUNE,
  DATA_TREE_NODE_TYPE_FIELD_AMPLITUDE,
  /* envelope fields */
  DATA_TREE_NODE_TYPE_FIELD_ATTACK,
  DATA_TREE_NODE_TYPE_FIELD_DECAY_1,
  DATA_TREE_NODE_TYPE_FIELD_DECAY_2,
  DATA_TREE_NODE_TYPE_FIELD_RELEASE,
  DATA_TREE_NODE_TYPE_FIELD_SUSTAIN,
  DATA_TREE_NODE_TYPE_FIELD_RATE_KEYSCALING,
  DATA_TREE_NODE_TYPE_FIELD_LEVEL_KEYSCALING,
  /* sequencer fields */
  DATA_TREE_NODE_TYPE_FIELD_KEY_SIGNATURE,
  DATA_TREE_NODE_TYPE_FIELD_TIME_SIGNATURE,
  DATA_TREE_NODE_TYPE_FIELD_TEMPO,
  DATA_TREE_NODE_TYPE_FIELD_SWING,
  DATA_TREE_NODE_TYPE_FIELD_PATTERN,
  /* pattern fields */
  DATA_TREE_NODE_TYPE_FIELD_NUMBER_OF_BARS,
  DATA_TREE_NODE_TYPE_FIELD_STEP,
  /* step fields */
  DATA_TREE_NODE_TYPE_FIELD_NOTE_1,
  DATA_TREE_NODE_TYPE_FIELD_NOTE_2,
  DATA_TREE_NODE_TYPE_FIELD_NOTE_3,
  DATA_TREE_NODE_TYPE_FIELD_NOTE_4,
  DATA_TREE_NODE_TYPE_FIELD_MODULATION,
  DATA_TREE_NODE_TYPE_FIELD_VOLUME,
  DATA_TREE_NODE_TYPE_FIELD_BRIGHTNESS,
  DATA_TREE_NODE_TYPE_FIELD_ARPEGGIO,
  DATA_TREE_NODE_TYPE_FIELD_PITCH_SWEEP,
  DATA_TREE_NODE_TYPE_FIELD_VIBRATO,
  DATA_TREE_NODE_TYPE_FIELD_TREMOLO,
  DATA_TREE_NODE_TYPE_FIELD_WOBBLE,
  /* arpeggio, lfo, and sweep fields */
  DATA_TREE_NODE_TYPE_FIELD_MODE,
  DATA_TREE_NODE_TYPE_FIELD_DEPTH,
  DATA_TREE_NODE_TYPE_FIELD_SPEED,
  /* attributes */
  DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_SAMPLING,
  DATA_TREE_NODE_TYPE_ATTRIBUTE_EXPORT_BITRES,
  DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_SYSTEM,
  DATA_TREE_NODE_TYPE_ATTRIBUTE_TUNING_FORK,
  /* values */
  DATA_TREE_NODE_TYPE_VALUE_INTEGER,
  DATA_TREE_NODE_TYPE_VALUE_FLOAT,
  DATA_TREE_NODE_TYPE_VALUE_STRING
};

typedef struct data_tree_node
{
  int   type;
  char* value;

  struct data_tree_node*  child;
  struct data_tree_node*  sibling;
} data_tree_node;

/* function declarations */
short int       data_tree_node_init(data_tree_node* node);
data_tree_node* data_tree_node_create();
short int       data_tree_node_deinit(data_tree_node* node);
short int       data_tree_node_destroy(data_tree_node* node);
short int       data_tree_node_destroy_tree(data_tree_node* node);

#endif
