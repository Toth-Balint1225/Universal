#include "lsn.h"
#include <rgx.h>

#include <unitest.h>

// #define PRINT

int test_integer(void)
{
	lison_t* lison = lsn_integer(69);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	int diff = lison->value.integer - 69;
	lsn_delete(&lison);
	return diff;
}

int test_floating(void)
{
	lison_t* lison = lsn_float(4.20);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	float diff = lison->value.lsn_float- 4.20;
	lsn_delete(&lison);
	return (int)diff;
}

int test_string(void)
{
	char* src = "Hello World!";
	lison_t* lison = lsn_string(src);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	int res = strcmp(lison->value.string, src);
	lsn_delete(&lison);
	return res;
}

int test_tag(void)
{
	char* src = "date";
	lison_t* lison = lsn_tag(src);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	int res = strcmp(lison->value.tag, src);
	lsn_delete(&lison);
	return res;
}

int test_string_str(void)
{
	str_t src = { .data = "asd asdasdasd asdasdasd", .len = 3 };
	lison_t* lison = lsn_string_str(&src);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	int res = strcmp(lison->value.string, "asd");
	lsn_delete(&lison);
	return res;
}

int test_tag_str(void)
{
	str_t src = { .data = "date", .len = 4 };
	lison_t* lison = lsn_tag_str(&src);
#ifdef PRINT
	lsn_print(lison);
	printf("\n");
#endif
	int res = strcmp(lison->value.tag, "date");
	lsn_delete(&lison);
	return res;
}

int test_object(void)
{
	lison_t* object;
	{
		lison_t* tag = lsn_tag("name");
		lison_t* name = lsn_string("John Doe");
		lison_list_t list = {NULL, NULL};
		lsn_lst_init(&list);
		lsn_lst_append(&list, tag);
		lsn_lst_append(&list, name);
		object = lsn_object(list);
	}

#ifdef PRINT
	lsn_print(object);
	printf("\n");
#endif
	int res = !object->value.object.head;

	lsn_delete(&object);

	return res;
}

int test_strnum(void)
{
	char* text = "12.3 asdasdasd lololool";
	str_t str_f = {
		.data = text,
		.len = 4,
	};
	str_t str_i = {
		.data = text,
		.len = 2,
	};
	
	float res_f = str_to_float(&str_f);
	float res_i = str_to_int(&str_i);
	
	
	return (int)(res_f - 12.3) + (res_i - 12);
}

int test_tokenizer(void)
{
	lsn_token_stream_t stream = { .head = NULL, .tail = NULL };
	lsn_ts_init(&stream);
	lsn_tokenize("(:height 165.5 :age 22 :name 'John')", &stream);
#ifdef PRINT
	lsn_ts_print(&stream);
#endif
	lsn_ts_delete(&stream);
	return 0;
}

int test_comment(void)
{
	lsn_token_stream_t stream = { .head = NULL, .tail = NULL };
	lsn_ts_init(&stream);
	// same as before, but age is commented out
	lsn_tokenize("(:height 165.5 (* :age 22 *) :name 'John')", &stream);
#ifdef PRINT
	lsn_ts_print(&stream);
#endif
	lsn_ts_delete(&stream);
	return 0;
}

int test_parse(void)
{
	lsn_token_stream_t stream = { .head = NULL, .tail = NULL };
	lsn_ts_init(&stream);
	lsn_tokenize("(:name 'Jacob Gipsz' (*animals are optional*) :animals ())", &stream);
	lsn_parse_res_t res = lsn_p_object(stream.head);
	int test = res.stream->value.tag == LSN_TKN_EOF ? 0 : 1;
#ifdef PRINT
	lsn_ts_print(&stream);
	lsn_print(res.lison);
	printf("\n");
#endif
	lsn_delete(&res.lison);
	lsn_ts_delete(&stream);
	return test;
}

int test_complex(void)
{
	lsn_token_stream_t stream = { .head = NULL, .tail = NULL };
	lsn_ts_init(&stream);
	lsn_tokenize("(:persons  ( (* First Person *) (:name 'John' (* comments can be anywhere *) :age 22 :height 165) (* Someone Else *) (:name 'Andrew Sharp' :age 27 :height 195 :workplaces  ((:name 'University of Pannonia')))))", &stream);
	lsn_parse_res_t res = lsn_p_object(stream.head);
	int test = res.stream->value.tag == LSN_TKN_EOF ? 0 : 1;
#ifdef PRINT
	lsn_ts_print(&stream);
	lsn_print(res.lison);
	printf("\n");
#endif
	lsn_delete(&res.lison);
	lsn_ts_delete(&stream);
	return test;
}

int test_compile(void)
{
	lison_t* lison = lsn_compile("(*Test*) (:name 'John' :height 165.4 :cars 1 :address (:postcode '9082' :city 'New York' :road '9th avenue' :house 10))");
	int res = lison ? 0 : 1;
#ifdef PRINT
	lsn_print(lison);
#endif
	lsn_delete(&lison);
	return res;
}

int test_compile_error_no_end(void)
{
	lison_t* lison = lsn_compile("(*Test*) (:name 'John' :height 165.4 :cars 1 :address (:postcode '9082' :city 'New York' :road '9th avenue' :house 10 (*) ) (*missing paren here*)");
	int res = lison ? 1 : 0;
#ifdef PRINT
	lsn_print(lison);
#endif
	lsn_delete(&lison);
	return res;
}

int test_compile_error_invalid_token(void)
{
	lison_t* lison = lsn_compile("(*Test*) (:name 'John' (*should fail here*) hello (*) :height 165.4 :cars 1 :address (:postcode '9082' :city 'New York' :road '9th avenue' :house 10)");
	int res = lison ? 1 : 0;
#ifdef PRINT
	lsn_print(lison);
#endif
	lsn_delete(&lison);
	return res;
}

int test_serde(void)
{
	return 1;
}

/*
  TODO:
  - pretty print lison
  - serialise / deserialise
  - convenience functions: lsn_is_empty(), ...
  - STR library with slices and strings probably (conversions, transformations, ...)
 */

RUN_TESTS(
	TEST (integer),
	TEST (floating),
	TEST (string),
	TEST (tag),
	TEST (string_str),
	TEST (tag_str),
	TEST (object),
	TEST (strnum),
	TEST (tokenizer),
	TEST (comment),
	TEST (parse),
	TEST (complex),
	TEST (compile),
	TEST (compile_error_no_end),
	TEST (serde),
)
