#include "rgx.h"
#include <unitest.h>

int test_compile()
{
	regex_t* rgx = rgx_compile("(a|1)*");
	if (rgx != NULL) {
		rgx_delete(&rgx);
		return 0;
	} else {
		return 1;
	}
}

int test_compile_err()
{
	regex_t* rgx = rgx_compile("(a|b*");
	if (rgx == NULL) {
		rgx_delete(&rgx);
		return 0;
	} else {
		return 1;
	}
}

int test_match()
{
	str_t match = rgx_match_src("abbaxy", "(ab | ba)*");
	if (match.len == 4)
		return 0;
	else 
		return 1;
}

int test_match_err()
{
	str_t match = rgx_match_src("cdabbxy", "(ab | ba+");
	if (match.len == 0)
		return 0;
	else 
		return 1;
}

int test_accept()
{
	bool acc = rgx_accept_src("abba", "(ab|ba)*");
	return !acc;
}

int test_accept_err()
{
	bool acc = rgx_accept_src("asd", "(ab|ba)*");
	return acc;
}

int test_tkn_escape(void)
{
	token_stream_t stream = {NULL, NULL};
	rgx_ts_init(&stream);
	rgx_tokenize("\\+ \\( \\) \\* \\| \\q", &stream);
	// rgx_print_stream(&stream);
	rgx_ts_delete(&stream);
	return 0;
}

int test_quote_set(void)
{
	regex_t* quote = rgx_quote_set();
	bool single = rgx_accept("'", quote);
	bool doubl = rgx_accept("\"", quote);
	bool back = rgx_accept("`", quote);
	/* rgx_print_regex(quote, 0); */
	/* printf("%d %d %d\n", single, doubl, back); */
	rgx_delete(&quote);
	return 1 - (single && doubl && back);
}

RUN_TESTS(
	TEST (compile), 
	TEST (compile_err), 
	TEST (match), 
	TEST (accept), 
	TEST (match_err), 
	TEST (accept_err), 
	TEST (tkn_escape),
	TEST (quote_set),
)
