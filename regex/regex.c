#include "rgx.h"
#include <stdio.h>
#include <string.h>

match_res_t match_char(char* src, char c)
{
	if (*src == c) return (match_res_t){ .succ = true, .rem = src + 1};
	else		   return (match_res_t){ .succ = false, .rem = src };
}

match_res_t match_union(char* src, match_res_t a, match_res_t b)
{
	if (a.succ)		 return (match_res_t) { .succ = true, .rem = a.rem };
	else if (b.succ) return (match_res_t) { .succ = true, .rem = b.rem };
	else			 return (match_res_t) { .succ = false, .rem = src };
}


match_res_t match_concat(char* src, const regex_t* a, const regex_t* b)
{
	match_res_t res_a = rgx_match_impl(src, a);
	if (res_a.succ)
	{
		match_res_t res_b = rgx_match_impl(res_a.rem, b);
		if (res_b.succ)
			return (match_res_t) { .succ = true, .rem = res_b.rem };
	}
	return (match_res_t) { .succ = false, .rem = src };
}

match_res_t match_star(char* src, const regex_t* star)
{
	match_res_t intermed = { .succ = true, .rem = src};
	do 
	{
		intermed = rgx_match_impl(intermed.rem, star);
	} while (intermed.succ);
	return (match_res_t) { .succ = true, .rem = intermed.rem };
}

match_res_t match_char_set(char* src)
{
	// TODO: use the power of ASCII
	const char* characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz.:,?;-!%@&$/=()<>[]";
	bool searching = true;
	size_t i = 0;
	while (searching && i < strlen(characters))
	{
		if (characters[i] == *src)
			return (match_res_t) { .succ = true, .rem = src + 1 };
		i++;
	}
	return (match_res_t) { .succ = false, .rem = src };
}

match_res_t match_whitespace_set(char* src)
{
	const char* whitespaces = " \t\n";
	bool searching = true;
	size_t i = 0;
	while (searching && i < strlen(whitespaces))
	{
		if (whitespaces[i] == *src)
			return (match_res_t) { .succ = true, .rem = src + 1 };
		i++;
	}
	return (match_res_t) { .succ = false, .rem = src };
}

match_res_t match_digit_set(char* src)
{
	const char* digits = "0123456789";
	bool searching = true;
	size_t i = 0;
	while (searching && i < strlen(digits))
	{
		if (digits[i] == *src)
			return (match_res_t) { .succ = true, .rem = src + 1 };
		i++;
	}
	return (match_res_t) { .succ = false, .rem = src };
}

match_res_t match_quote_set(char* src)
{
	const char* quotes = "'\"`";
	bool searching = true;
	size_t i = 0;
	while (searching && i < strlen(quotes))
	{
		if (quotes[i] == *src)
			return (match_res_t) { .succ = true, .rem = src + 1 };
		i++;
	}
	return (match_res_t) { .succ = false, .rem = src };
}

// except whitespace
match_res_t match_wildcard(char* src)
{
	const char* whitespaces = " \t\n";
	bool searching = true;
	size_t i = 0;
	while (searching && i < strlen(whitespaces))
	{
		if (whitespaces[i] == *src)
			return (match_res_t) { .succ = false, .rem = src };
		i++;
	}
	return (match_res_t) { .succ = true, .rem = src + 1 };
}

match_res_t match_plus(char* src, const regex_t* plus)
{
	match_res_t first = rgx_match_impl(src, plus);
	if (first.succ) return match_star(first.rem, plus);
	else            return (match_res_t) { .succ = false, .rem = src };
}

match_res_t rgx_match_impl(char* src, const regex_t* regex)
{
	switch (regex->type)
	{
	case Character:
		return match_char(src, regex->value.character);
	case Union:
		return match_union(src, rgx_match_impl(src, regex->value.uni.a), rgx_match_impl(src, regex->value.uni.b));
	case Concat:
		return match_concat(src, regex->value.concat.a, regex->value.concat.b);
	case Star:
		return match_star(src, regex->value.star);
	case CharSet:
		return match_char_set(src);
	case WhitespaceSet:
		return match_whitespace_set(src);
	case DigitSet:
		return match_digit_set(src);
	case QuoteSet:
		return match_quote_set(src);
	case Wildcard:
		return match_wildcard(src);
	case Plus:
		return match_plus(src, regex->value.plus);
	default:
		return (match_res_t) { .succ = false, .rem = src }; 
	}
}

regex_t* rgx_character(char c)
{
	LOG("[REGEX] Allocating Character with %c\n", c);
	regex_t* res = malloc(sizeof(regex_t));
	res->type = Character,
	res->value.character = c;
	return res;
}

regex_t* rgx_concat(regex_t* a, regex_t* b)
{
	LOG("[REGEX] Allocating Concat\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = Concat,
	res->value.concat = (regex_pair_t) { .a = a, .b = b};
	return res;
}

regex_t* rgx_union(regex_t* a, regex_t* b)
{
	LOG("[REGEX] Allocating Union\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = Union;
	res->value.uni = (regex_pair_t) { .a = a, .b = b};
	return res;
}

regex_t* rgx_star(regex_t* star)
{
	LOG("[REGEX] Allocating Star\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = Star;
	res->value.star = star;
	return res;
}

regex_t* rgx_char_set()
{
	LOG("[REGEX] Allocating Character Set\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = CharSet;
	return res;
}

regex_t* rgx_digit_set()
{
	LOG("[REGEX] Allocating Digit Set\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = DigitSet;
	return res;
}

regex_t* rgx_whitespace_set()
{
	LOG("[REGEX] Allocating Whitespace Set\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = WhitespaceSet;
	return res;
}

regex_t* rgx_quote_set()
{
	LOG("[REGEX] Allocating Quote Set\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = QuoteSet;
	return res;
}

regex_t* rgx_plus(regex_t* plus)
{
	LOG("[REGEX] Allocating Plus\n");
	regex_t* res = malloc(sizeof(regex_t));
	res->type = Plus;
	res->value.plus = plus;
	return res;
}

void rgx_delete(regex_t** regex)
{
	if (!*regex)
		return;
	switch ((*regex)->type)
	{
	case Character:
		break;
	case Union:
	{
		rgx_delete(&(*regex)->value.uni.a);
		rgx_delete(&(*regex)->value.uni.b);
		break;
	}
	case Concat:
	{
		rgx_delete(&(*regex)->value.concat.a);
		rgx_delete(&(*regex)->value.concat.b);
		break;
	}
	case Star:
	{
		rgx_delete(&(*regex)->value.star);
		break;
	}
	case Plus:
	{
		rgx_delete(&(*regex)->value.plus);
		break;
	}
	default:
		break;
	}
	LOG("[REGEX] Deleting (%d)\n", (*regex)->type);
	free(*regex);
	*regex = NULL;
}

bool rgx_accept(const char* src, const regex_t* regex)
{
	if (!regex)
		return false;
	match_res_t res = rgx_match_impl((char*)src, regex);
	size_t length = (size_t)(res.rem - src);
	if (length == strlen(src)) return res.succ;
	else                       return false;
}

bool rgx_accept_src(const char* src, const char* regex)
{
	regex_t* rgx = rgx_compile(regex);
	if (!rgx) return false;
	bool res = rgx_accept(src, rgx);
	rgx_delete(&rgx);
	return res;
}

str_t rgx_match(const char* src, const regex_t* regex)
{
	if (!regex) return (str_t) {.data = (char*)src, .len = 0};
	match_res_t res = rgx_match_impl((char*)src, regex);
	size_t length = res.rem - src;
	return (str_t) {.data = (char*)src, .len = length};
}

str_t rgx_match_src(const char* src, const char* regex)
{
	regex_t* rgx = rgx_compile(regex);
	if (!rgx) return (str_t) {.data = (char*)src, .len = 0};
	str_t res = rgx_match(src, rgx);
	rgx_delete(&rgx);
	return res;
}

void print_tab(unsigned tab)
{
	for (unsigned i=0;i<tab;i++)
		printf(" ");
	fflush(stdout);
}

void rgx_print_regex(regex_t* regex, unsigned tab)
{
	if (!regex)
	{
		printf("Empty Regex\n");
		return;
	}
	print_tab(tab);
	switch (regex->type)
	{
        case Character:
			printf("Char {%c}\n", regex->value.character);
			break;
        case Union:
			printf("Union {\n");
			rgx_print_regex(regex->value.uni.a, tab + 2);
			rgx_print_regex(regex->value.uni.b, tab + 2);
			print_tab(tab);
			printf("}\n");
			break;
        case Concat:
			printf("Concat {\n");
			rgx_print_regex(regex->value.concat.a, tab + 2);
			rgx_print_regex(regex->value.concat.b, tab + 2);
			print_tab(tab);
			printf("}\n");
			break;
        case Star:
			printf("Star {\n");
			rgx_print_regex(regex->value.star, tab + 2);
			print_tab(tab);
			printf("}\n");
			break;
        case Plus:
			printf("Plus {\n");
			rgx_print_regex(regex->value.plus, tab + 2);
			print_tab(tab);
			printf("}\n");
			break;
        case CharSet:
			printf("CharSet\n");
			break;
        case DigitSet:
			printf("DigitSet\n");
			break;
        case WhitespaceSet:
			printf("WhitespaceSet\n");
			break;
        case QuoteSet:
			printf("QuoteSet\n");
			break;
        case Wildcard:
			break;
        }
}

void rgx_print_res(match_res_t res)
{
	printf("Match result: [%d, %s]\n", res.succ, res.rem);
}
