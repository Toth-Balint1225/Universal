#ifndef LISON_H
#define LISON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <unilog.h>
#include <rgx.h>
#include <str.h>

// Namespace prefix: lsn_

/*
  example, should be valid:
  (:persons (
    (* First Person *)
    (:name 'John' (* comments can be anywhere *)
	 :age 22
	 :height 165)
    (* Someone Else *)
	(:name 'Andrew Sharp'
	 :age 27 (* idk precisely *)
	 :height 195
	 :workplaces (
	   (:name 'University of Pannonia')))))
 */

/** 
 *
 * <object> ::= '(' <list> ')'
 *            | <string>
 *            | <integer>
 *            | <float>
 *            | <tag>
 *            ;
 *
 * <list> ::= [ <object> ]
 *          | empty
 *          ;
 *
 * <string> = /'(\c|\w|\d)*'/;
 * <integer> = /\d+/;
 * <float> = /\d+.\d+/;
 * <tag> = /:(\c|\d)+/;
 * 
 * Comment ::= '(*' '*)'
 */

// lison list
struct _lison_t;
typedef struct _lison_node_t
{
	struct _lison_node_t* next;
	struct _lison_node_t* prev;
	struct _lison_t* value;

} lison_node_t;

typedef struct _lison_list_t
{
	lison_node_t* head;
	lison_node_t* tail;
} lison_list_t;

// AST
typedef enum _lison_tag_t
{
	LSN_Object,
	LSN_String,
	LSN_Integer,
	LSN_Float,
	LSN_Tag,
} lison_tag_t;

typedef struct _lison_t
{
	lison_tag_t tag;
	union
	{
		lison_list_t object;
		char* string; // these strings must be dynamically allocated and stored by the lison_t
		int32_t integer;
		float lsn_float;
		char* tag;
	} value;
} lison_t;

typedef enum _lsn_token_tag_t
{
	LSN_TKN_CommStart = 0,  // comments must be before parentheses.
	LSN_TKN_CommEnd = 1,
	LSN_TKN_LParen = 2,
	LSN_TKN_RParen = 3,
	LSN_TKN_Whitespace = 4,
	LSN_TKN_Float = 5, // integer must be after float
	LSN_TKN_Integer = 6,
	LSN_TKN_String = 7,
	LSN_TKN_Tag = 8,
	LSN_TKN_EOF = 9,
} lsn_token_tag_t;

typedef struct _lsn_token_t
{
	lsn_token_tag_t tag;
	union
	{
		int integer;
		float lsn_float;
		str_t string;
		str_t tag;
	} value;
} lsn_token_t;

typedef struct _lsn_token_node
{
	lsn_token_t value;
	struct _lsn_token_node* next;
	struct _lsn_token_node* prev;
} lsn_token_node_t;

typedef struct _lsn_token_stream
{
	lsn_token_node_t* head;
	lsn_token_node_t* tail;
} lsn_token_stream_t;

typedef struct _lsn_parse_res
{
	lsn_token_node_t* stream;
	lison_t* lison;
} lsn_parse_res_t;

// ast.c
// LiSON list driver code
void lsn_lst_init(lison_list_t* list);
void lsn_lst_delete(lison_list_t* list);
void lsn_lst_append(lison_list_t* list, lison_t* object);

// LiSON util
lison_t* lsn_string(char* str);
lison_t* lsn_tag(char* str);
lison_t* lsn_string_str(const str_t* str);
lison_t* lsn_tag_str(const str_t* str);
lison_t* lsn_integer(int value);
lison_t* lsn_float(float value);
lison_t* lsn_object(lison_list_t list);

// parser.c
// Token Stream driver
void lsn_ts_init(lsn_token_stream_t* stream);
void lsn_ts_delete(lsn_token_stream_t* stream);
void lsn_ts_append(lsn_token_stream_t* stream, lsn_token_t tkn);
void lsn_ts_print(lsn_token_stream_t* stream);

// Parser util
void lsn_tokenize(char* src, lsn_token_stream_t* stream);
void lsn_print_token(const lsn_token_t* tkn);

// Recursive descent parser functions
lsn_parse_res_t lsn_p_object(lsn_token_node_t* lkd);
lsn_parse_res_t lsn_p_list(lsn_token_node_t* lkd);
lsn_token_node_t* lsn_p_expect(lsn_token_node_t* lkd, lsn_token_tag_t tkn);

// API
// parser.c
lison_t* lsn_compile(char* src);
// ast.c
void lsn_delete(lison_t** object);
void lsn_print(lison_t* object);
// serde.c
lison_t* lsn_deserialize(char* filepath);
void lsn_serialize(char* filepath, lison_t* lison);

#endif //LISON_H
