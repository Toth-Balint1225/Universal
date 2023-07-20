#include "str.h"
#include <unitest.h>

// #define PRINT

int test_from_cstr(void)
{
	str_t str = str_from_cstr("Hello World!");
	return str.len - 12;
}

int test_from_cstr_ln(void)
{
	char* src = "Hello World!";
	str_t str = str_from_cstr_ln(src, 5);
    return str.len - 5; 
}

int test_from_cstr_ln_error(void)
{
	char* src = "Hello World!";
	str_t str = str_from_cstr_ln(src, 20);
    return str.len; 
}

int test_to_cstr(void)
{
	str_t str = str_from_cstr("Hello World!");
	char* cstr = str_to_cstr(&str);
#ifdef PRINT
	printf("%s\n", cstr);
#endif
	free(cstr);
	return cstr? 0 : 1;
}

int test_to_cstr_error(void)
{
	str_t str = str_from_cstr("Hello World!");
	str.len = 30;
	char* cstr = str_to_cstr(&str);
	free(cstr);
	return cstr? 1 : 0;
}

int test_to_float(void)
{
	char* text = "12.3 asdasdasd lololool";
	str_t str_f = {
		.data = text,
		.len = 4,
	};
	float res_f = str_to_float(&str_f);
	return (int)(res_f - 12.3); 
}

int test_to_int(void)
{
	char* text = "12.3 asdasdasd lololool";
	str_t str_i = {
		.data = text,
		.len = 2,
	};
	float res_i = str_to_int(&str_i);
	return res_i - 12;
}

int test_to_float_error(void)
{
	char* text = "12.3 asdasdasd lololool";
	str_t str_f = {
		.data = text,
		.len = 4,
	};
	str_f.len = 100;
	float res_f = str_to_float(&str_f);
	return (int)res_f; 
}

int test_to_int_error(void)
{
	char* text = "12.3 asdasdasd lololool";
	str_t str_i = {
		.data = text,
		.len = 2,
	};
	str_i.len = 100;
	float res_i = str_to_int(&str_i);
	return res_i;
}

RUN_TESTS(
	TEST (from_cstr),
	TEST (from_cstr_ln),
	TEST (from_cstr_ln_error),
	TEST (to_cstr),
	TEST (to_cstr_error),
	TEST (to_float),
	TEST (to_int),
	TEST (to_float_error),
	TEST (to_int_error),
)
