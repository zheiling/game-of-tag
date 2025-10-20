#include "json.h"
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static f_type detect_field_type(char *cur_field, i_field_t types[]) {
  for (int i = 0; types[i].name; i++) {
    if (!strcmp(types[i].name, cur_field)) {
      return types[i].type;
    }
  }
  return ERR;
}

static int parse_int(char *str, o_field_t *field) {
  int l_data = 0;
  long int ll_data;
  int i = 0;
  while (isspace(str[i])) i++;

  for (; isalnum(str[i]); i++) {
    l_data = l_data * 10 + str[i] - '0';
  }

  field->data = malloc(sizeof(int));
  memcpy(field->data, &l_data, sizeof(int));
  return i;
}

static int parse_long_int(char *str, o_field_t *field) {
  long int l_data = 0;
  int i = 0;
  while (isspace(str[i])) i++;

  for (; isalnum(str[i]); i++) {
    l_data = l_data * 10 + str[i] - '0';
  }

  field->data = malloc(sizeof(long int));
  memcpy(field->data, &l_data, sizeof(long int));
  return i;
}

static int parse_char_arr(char *str, o_field_t *field) {
  int i = 0;
  int arr_idx = 0;
  int count = 0;
  while (str[i++] != '[')
    ;
  i++;
  for (int li = i; str[li] != ']'; li++) {
    if (str[li] == ',')
      count++;
  }
  char *l_data = malloc(sizeof(char) * (count + 1));
  unsigned char c = 0;
  for (; str[i] != ']'; i++) {
    switch (str[i]) {
    case ',':
      l_data[arr_idx++] = c;
      c = 0;
      continue;
    default:
      if (isspace(str[i]))
        continue;
      c = c * 10 + str[i] - '0';
    }
  }
  while (str[i] != ',')
    i++;
  l_data[arr_idx++] = c;
  l_data[arr_idx] = 0;
  field->data = (void *)l_data;
  return i;
}

static int parse_data(char *str, f_type type, o_field_t *field) {
  switch (type) {
  case J_INT:
    return parse_int(str, field);
  case J_A_CHAR:
    return parse_char_arr(str, field);
  case ERR:
  case J_CHAR:
  case J_STRING:
  case J_A_INT:
    break;
  case J_TIME:
    return parse_long_int(str, field);
    break;
  }
  return 0;
}

o_field_t *parse_json_field(char *str, int s_size, i_field_t types[]) {
  char name[64];
  static int idx = 0;
  int nidx = 0;
  o_field_t *field_ptr;

  if (idx == 0) {
    for (; str[idx] != '{' && (idx < s_size); idx++)
      ;
  }

  if (idx == s_size) {
    errno = 1; // file is not correct
    return NULL;
  }

  for (; str[idx] != '\"' && (idx < s_size); idx++)
    ;

  if (idx == s_size) {
    errno = 2; // possible end of file
    return NULL;
  }
  idx++;

  for (; str[idx] != '\"' && (idx < s_size); idx++) {
    name[nidx++] = str[idx];
  }
  name[nidx] = 0;
  for (; str[idx] != ':' && (idx < s_size); idx++)
    ;
  idx++;
  f_type cur_type = detect_field_type(name, types);
  field_ptr = malloc(sizeof(o_field_t));
  field_ptr->name = malloc(sizeof(char) * (strlen(name) + 1));
  strcpy(field_ptr->name, name);
  idx += parse_data(str+idx, cur_type, field_ptr);
  return field_ptr;
}

#ifdef __cplusplus
  }
#endif
