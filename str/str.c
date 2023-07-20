#include "str.h"

str_t str_from_cstr(cstring_t cstr)
{
	return (str_t) {
		.data = cstr,
		.len = strlen(cstr)
	};
}

str_t str_from_cstr_ln(cstring_t cstr, size_t len)
{
	if (len > strlen(cstr))
		return (str_t) { .data = cstr, .len = 0 };
	return (str_t) {
		.data = cstr,
		.len = len,
	};
}

cstring_t str_to_cstr(str_t* str)
{
	if (str->len > strlen(str->data))
		return NULL;
	char* buff = (char*)malloc(str->len + 1);
	strncpy(buff, str->data, str->len);
	buff[str->len] = 0;
	return buff;
}

void str_print(const str_t* str)
{
	str_fprint(stdout, str);
}

void str_fprint(FILE* out, const str_t* str)
{
	if (str->len > strlen(str->data))
		return;
	for (size_t i=0;i<str->len;i++)
		fprintf(out,"%c",str->data[i]);
}

float str_to_float(str_t* str)
{
	char* buff = str_to_cstr(str);
	if (!buff)
		return 0.0;
	float res = atof(buff);
	free(buff);
	return res;
}

int str_to_int(str_t* str)
{
	char* buff = str_to_cstr(str);
	if (!buff)
		return 0;
	float res = atoi(buff);
	free(buff);
	return res;
}
