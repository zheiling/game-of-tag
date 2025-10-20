#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ERR,
  J_INT,    // int
  J_TIME,   // time_t
  J_CHAR,   // char
  J_STRING, // char[]
  J_A_INT,  // int[]
  J_A_CHAR, // char[]
} f_type;

typedef struct {
  char *name;
  void *data; // parsed data;
  f_type type;
} o_field_t;

typedef struct {
  char *name;
  f_type type;
} i_field_t;

o_field_t *parse_json_field(char *str, int s_size, i_field_t types[]);
void clear_json_field(o_field_t *field);

#ifdef __cplusplus
}
#endif

#endif
