#ifndef REGEX_H
#define REGEX_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <str.h>

#include <unilog.h>

/**
 * Tóth Bálint, University of Pannonia
 * 2023.
 *
 * Regular expression base library.
 *
 * Basic regular expressions: 
 *	characters: a,b...
 *	concatenation: ab
 *	conjunction: a|b
 *	Kleene-star: a*
 *
 * Extended regular expressions:
 *	negating a regex: ~(a|b)
 *	character classes: \c := characters, \d := digits, \w := whitespaces
 *	anchors: ^ab$
 *	escapement: \|, \*
 *	extra quantifiers: a+ := aa*
 *	universal character: _ (just an epsilon transition)
 */

/**
 * Enum to tag the regex union.
 */
typedef enum _regex_type_t
{
	// BASIC REGEX
    Character,         // 0
    Union,             // 1
    Concat,            // 2
    Star,              // 3
	// EXTENDED REGEX
	Plus,              // 4
	// Negate,
	CharSet,           // 5
	DigitSet,          // 6
	WhitespaceSet,     // 7
	QuoteSet,          // 8
	Wildcard,          // 9
} regex_type_t;

/**
 * Pair of regular expression pointers.
 */
typedef struct _regex_t regex_t;
typedef struct _regex_pair_t
{
	regex_t* a;
	regex_t* b;
} regex_pair_t;

/**
 * Tagged Union to describe a regular expression.
 * Regex has a tag (regex_type_t), and the union of
 * the different regex contents to create a tree 
 * structure with pointers.
 */
typedef struct _regex_t
{
	regex_type_t type;
	union
	{
		char character;
		regex_pair_t uni;
		regex_pair_t concat;
		struct _regex_t* star;
		struct _regex_t* plus;
	} value;
} regex_t;

/**
 * Multiple return value type for the match functions.
 * Contains a success flag and the remainder of the base
 * string.
 */
typedef struct _match_res_t
{
	bool succ;
	char* rem;
} match_res_t;

// PRIVATE --------------------------------------------------
/**
 * Implementation of the matching of a regular expression.
 */
match_res_t rgx_match_impl(char* src, const regex_t* regex);

// API --------------------------------------------------
/**
 * Function that applies a regular expression to a string source.
 * Returns true if the finite-state machine that is equivalent
 * to regex accepts the source. Otherwise the function returns 
 * false.
 * Errors:
 * - if either src or regex are NULL, the result will be false.
 */
bool rgx_accept(const char* src, const regex_t* regex);

/**
 * Function that applies a regular expression to a string source.
 * Requires the source of the regular expression with the below 
 * syntax.
 * Returns true if the finite-state machine that is equivalent
 * to regex accepts the source. Otherwise the function returns 
 * false.
 * Errors:
 * - if either src or regex are NULL, the result will be false.
 */
bool rgx_accept_src(const char* src, const char* regex);

/**
 * Function that applies a regular expression to a string and gets
 * the first n characters that the regular expression generates.
 * Returns the accepted substring in a str_t slice form. The slice
 * does not allocate memory, meaning it is only valid if the source
 * string still exists in memory. (Stack allocation recommended).
 * Errors:
 * - if either src or regex are NULL, the result will be a slice 
 *   with NULL data and zero length.
 */
str_t rgx_match(const char* src, const regex_t* regex);

/**
 * Function that applies a regular expression to a string and gets
 * the first n characters that the regular expression generates.
 * Requires the source of the regular expression with the below 
 * syntax.
 * Returns the accepted substring in a str_t slice form. The slice
 * does not allocate memory, meaning it is only valid if the source
 * string still exists in memory. (Stack allocation recommended).
 * Errors:
 * - if either src or regex are NULL, the result will be a slice 
 *   with NULL data and zero length.
 */
str_t rgx_match_src(const char* src, const char* regex);

/**
 * Function to free the memory of a regular expression.
 * Must be called after using any method of generating a regex
 * structure.
 */
void rgx_delete(regex_t** regex);

// UTIL --------------------------------------------------

/**
 * Function to create a regular expression corresponding to 
 * the given character.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_character(char c);

/**
 * Function to create a regular expression corresponding to 
 * the concatenation of the given regular expressions.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_concat(regex_t* a, regex_t* b);

/**
 * Function to create a regular expression corresponding to 
 * the union (conjunction) of the given regular expressions.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_union(regex_t* a, regex_t* b);

/**
 * Function to create a regular expression corresponding to 
 * the Kleene-star of the given regular expression.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_star(regex_t* star);

/**
 * Function to create a regular expression corresponding to 
 * the extended Kleene-star (At least 1 or more) of the given 
 * regular expression.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_plus(regex_t* star);

/**
 * Function to create a regular expression corresponding to 
 * the set of lower and uppercase ASCII characters.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_char_set();

/**
 * Function to create a regular expression corresponding to 
 * the set of arabic numerals.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_digit_set();

/**
 * Function to create a regular expression corresponding to 
 * the set of whitespaces: tab, space and newline.
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_whitespace_set();

/**
 * Function to create a regular expression corresponding to 
 * the set of Quotes: single ('), double ("), backtick (`).
 * Important: Dynamically allocates memory to store the regex.
 */
regex_t* rgx_quote_set();

/**
 * Free the memory of a regular expression.
 * Must be called to the root of the regex tree.
 */
void rgx_delete(regex_t** regex);

/**
 * Utility function to print a match_res_t struct.
 */
void rgx_print_res(match_res_t res);

/**
 * Utility function to print a regex tree.
 */
void rgx_print_regex(regex_t* regex, unsigned tab);

/**
 * Parser for string representation -> regex structure conversion
 *
 * precedence order () > * = + > ° > |
 * The language for basic regex:
 * <expression> ::= <term> <conjunction>;
 *
 * <conjunction> ::= [ '|' <term> ]
 *				   | <empty>
 *				   ;
 *
 * <term> ::= <factor> <concatenation>;
 *
 * <concatenation> ::= [ <factor> ] 
 *	                 | <empty>
 *	                 ;
 *
 * <factor> ::= <operand> <length_mod>
 *
 * <length_mod> ::= '*'
 *                | '+'
 *                | <empty>
 *                ;
 *
 * <operand> ::= '(' <expression> ')'
 *             | <character>
 *             | <char_set>
 *             | <digit_set>
 *             | <whitespace_set>
 *             | <quote_set>
 *             ;
 *
 * <character> ::= /\c|\q/;
 */
typedef enum _token_type
{
	Tkn_Character,        // 0
	Tkn_Bar,              // 1
	Tkn_LParen,           // 2
	Tkn_RParen,           // 3
	Tkn_Star,             // 4
	Tkn_Plus,             // 5
	Tkn_CharSet,          // 6
	Tkn_DigitSet,         // 7
	Tkn_WhitespaceSet,    // 8
	Tkn_QuoteSet,         // 9
	Tkn_EndOfInput,       // 10
} token_type;

typedef struct _token_t
{
	token_type type;

	union
	{
		char character;
	} value;
} token_t;

typedef struct _token_node
{
	token_t value;
	struct _token_node* next;
	struct _token_node* prev;
} token_node_t;

typedef struct _token_stream
{
	token_node_t* head;
	token_node_t* tail;
} token_stream_t;

typedef struct _parse_res
{
	token_node_t* stream;
	regex_t* regex;
} parse_res_t;

// token stream driver
void rgx_ts_init(token_stream_t* stream);
void rgx_ts_delete(token_stream_t* stream);
void rgx_ts_append(token_stream_t* stream, token_t tkn);

// conversion api
void rgx_tokenize(const char* src, token_stream_t* res);
regex_t* rgx_compile(const char* src);

// the parser functions
parse_res_t expression(token_node_t* lkd, regex_t* regex);
parse_res_t term(token_node_t* lkd, regex_t* regex);
parse_res_t factor(token_node_t* lkd, regex_t* regex);
parse_res_t operand(token_node_t* lkd, regex_t* regex);
parse_res_t length_mod(token_node_t* lkd, regex_t* inner);
parse_res_t conjunction(token_node_t* lkd, regex_t* regex);
parse_res_t concatenation(token_node_t* lkd, regex_t* regex);
token_node_t* expect(token_node_t* lkd, token_type tkn);

void rgx_print(const token_t* tkn);
void rgx_print_stream(const token_stream_t* stream);
void rgx_print_parse_res(parse_res_t* res);
#endif // REGEX_H
