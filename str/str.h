#ifndef STR_H
#define STR_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef char* cstring_t;

typedef struct _str_t
{
	char* data;
	size_t len;
} str_t;

// creation
// important: str_t is a slice, it cannot be freed
str_t str_from_cstr(cstring_t cstr);
str_t str_from_cstr_ln(cstring_t cstr, size_t len);
cstring_t str_to_cstr(str_t* str);

// display
void str_print(const str_t* str);
void str_fprint(FILE* out, const str_t* str);

// conversion
float str_to_float(str_t* str);
int str_to_int(str_t* str);

#endif // STR_H
