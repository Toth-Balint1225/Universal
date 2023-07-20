#include "rgx.h"
/**
 * Token Stream driver code
 */
void rgx_ts_delete(token_stream_t* stream)
{
	if (stream == NULL) return;
	token_node_t* del = stream->head;
	token_node_t* next = del;

	while (del != NULL)
	{
		next = del->next;
		LOG("[STREAM] Deleting token(%d)\n", del->value.type);
		free(del);
		del = next;
	} 
	stream->head = NULL;
	stream->tail = NULL;
}

void rgx_ts_init(token_stream_t* stream)
{
	if (!stream) return;
	if (stream->head)
		rgx_ts_delete(stream);

	// continue with init
	stream->head = (token_node_t*)malloc(sizeof(token_node_t));
	stream->head->next = NULL;
	stream->head->prev = NULL;
	stream->head->value.type = Tkn_EndOfInput;
	stream->tail = stream->head;
}

void rgx_ts_append(token_stream_t* stream, token_t tkn)
{
	token_node_t* new = 
		(token_node_t*)malloc(sizeof(token_node_t));
	new->value = tkn;
	if (stream->head == stream->tail)
	{
		new->prev = NULL;
		new->next = stream->tail;
		stream->head = new;
		stream->tail->prev = new;
	}
	else
	{
		stream->tail->prev->next = new;
		new->prev = stream->tail->prev;
		stream->tail->prev = new;
		new->next = stream->tail;
	}
}

void rgx_tokenize(const char* src, token_stream_t* res)
{
	rgx_ts_init(res);
	char* pointer = (char*)src;
	regex_t* lparen_regex         = rgx_character('(');
	regex_t* rparen_regex         = rgx_character(')');
	regex_t* star_regex           = rgx_character('*');
	regex_t* bar_regex            = rgx_character('|');
	regex_t* plus_regex           = rgx_character('+');
	regex_t* char_set_regex       = rgx_concat(rgx_character('\\'), rgx_character('c'));
	regex_t* digit_set_regex      = rgx_concat(rgx_character('\\'), rgx_character('d'));
	regex_t* whitespace_set_regex = rgx_concat(rgx_character('\\'), rgx_character('w'));
	regex_t* quote_set_regex      = rgx_concat(rgx_character('\\'), rgx_character('q'));
	regex_t* char_regex           = rgx_char_set();
	regex_t* digit_regex          = rgx_digit_set();
	regex_t* quote_regex          = rgx_quote_set();
	regex_t* whitespace_regex     = rgx_whitespace_set();
	regex_t* lparen_regex_esc     = rgx_concat(rgx_character('\\'), rgx_character('('));
	regex_t* rparen_regex_esc     = rgx_concat(rgx_character('\\'), rgx_character(')'));
	regex_t* star_regex_esc       = rgx_concat(rgx_character('\\'), rgx_character('*'));
	regex_t* bar_regex_esc        = rgx_concat(rgx_character('\\'), rgx_character('|'));
	regex_t* plus_regex_esc       = rgx_concat(rgx_character('\\'), rgx_character('+'));
	while (*pointer != 0)
	{
		match_res_t lparen_match         = rgx_match_impl(pointer, lparen_regex);
		if (lparen_match.succ)
		{
			pointer = lparen_match.rem;
			token_t lparen;
			lparen.type = Tkn_LParen;
			rgx_ts_append(res, lparen);
			continue;
		}
		match_res_t rparen_match         = rgx_match_impl(pointer, rparen_regex);
		if (rparen_match.succ)
		{
			pointer = rparen_match.rem;
			token_t rparen;
			rparen.type = Tkn_RParen;
			rgx_ts_append(res, rparen);
			continue;
		}
		match_res_t star_match           = rgx_match_impl(pointer, star_regex);
		if (star_match.succ)
		{
			pointer = star_match.rem;
			token_t star;
			star.type = Tkn_Star;
			rgx_ts_append(res, star);
			continue;
		}
		match_res_t bar_match            = rgx_match_impl(pointer, bar_regex);
		if (bar_match.succ)
		{
			pointer = bar_match.rem;
			token_t bar;
			bar.type = Tkn_Bar;
			rgx_ts_append(res, bar);
			continue;
		}
		match_res_t plus_match           = rgx_match_impl(pointer, plus_regex);
		if (plus_match.succ)
		{
			pointer = plus_match.rem;
			token_t plus;
			plus.type = Tkn_Plus;
			rgx_ts_append(res, plus);
			continue;
		}
		match_res_t char_set_match       = rgx_match_impl(pointer, char_set_regex);
		if (char_set_match.succ)
		{
			pointer = char_set_match.rem;
			token_t char_set;
			char_set.type = Tkn_CharSet;
			rgx_ts_append(res, char_set);
			continue;
		}
		match_res_t digit_set_match      = rgx_match_impl(pointer, digit_set_regex);
		if (digit_set_match.succ)
		{
			pointer = digit_set_match.rem;
			token_t digit_set;
			digit_set.type = Tkn_DigitSet;
			rgx_ts_append(res, digit_set);
			continue;
		}
		match_res_t whitespace_set_match = rgx_match_impl(pointer, whitespace_set_regex);
		if (whitespace_set_match.succ)
		{
			pointer = whitespace_set_match.rem;
			token_t whitespace_set;
			whitespace_set.type = Tkn_WhitespaceSet;
			rgx_ts_append(res, whitespace_set);
			continue;
		}
		match_res_t quote_set_match = rgx_match_impl(pointer, quote_set_regex);
		if (quote_set_match.succ)
		{
			pointer = quote_set_match.rem;
			token_t quote_set;
			quote_set.type = Tkn_QuoteSet;
			rgx_ts_append(res, quote_set);
			continue;
		}
		match_res_t character_match           = rgx_match_impl(pointer, char_regex);
		if (character_match.succ)
		{
			token_t character;
			character.type = Tkn_Character;
			character.value.character = *pointer;
			pointer = character_match.rem;
			rgx_ts_append(res, character);
			continue;
		}
		match_res_t digit_match          = rgx_match_impl(pointer, digit_regex);
		if (digit_match.succ)
		{
			token_t digit;
			digit.type = Tkn_Character;
			digit.value.character = *pointer;
			pointer = digit_match.rem;
			rgx_ts_append(res, digit);
			continue;
		}
		match_res_t quote_match = rgx_match_impl(pointer, quote_regex);
		if (quote_match.succ)
		{
			token_t quote;
			quote.type = Tkn_Character;
			quote.value.character = *pointer;
			pointer = quote_match.rem;
			rgx_ts_append(res, quote);
			continue;
		}
		match_res_t whitespace_match     = rgx_match_impl(pointer, whitespace_regex);
		if (whitespace_match.succ)
		{
			pointer = whitespace_match.rem;
			continue;
		}
		match_res_t lparen_esc_match = rgx_match_impl(pointer, lparen_regex_esc);
		if (lparen_esc_match.succ)
		{
			token_t esc;
			esc.type = Tkn_Character;
			esc.value.character = '(';
			pointer = lparen_esc_match.rem;
			rgx_ts_append(res, esc);
			continue;
		}
		match_res_t rparen_esc_match = rgx_match_impl(pointer, rparen_regex_esc);
		if (rparen_esc_match.succ)
		{
			token_t esc;
			esc.type = Tkn_Character;
			esc.value.character = ')';
			pointer = rparen_esc_match.rem;
			rgx_ts_append(res, esc);
			continue;
		}
		match_res_t star_esc_match = rgx_match_impl(pointer, star_regex_esc);  
		if (star_esc_match.succ)
		{
			token_t esc;
			esc.type = Tkn_Character;
			esc.value.character = '*';
			pointer = star_esc_match.rem;
			rgx_ts_append(res, esc);
			continue;
		}
		match_res_t bar_esc_match = rgx_match_impl(pointer, bar_regex_esc);   
		if (bar_esc_match.succ)
		{
			token_t esc;
			esc.type = Tkn_Character;
			esc.value.character = '|';
			pointer = bar_esc_match.rem;
			rgx_ts_append(res, esc);
			continue;
		}
		match_res_t plus_esc_match = rgx_match_impl(pointer, plus_regex_esc);  
		if (plus_esc_match.succ)
		{
			token_t esc;
			esc.type = Tkn_Character;
			esc.value.character = '+';
			pointer = plus_esc_match.rem;
			rgx_ts_append(res, esc);
			continue;
		}
		// nothing matches
		pointer += 1;
	}
	rgx_delete(&char_regex);
	rgx_delete(&digit_regex);
	rgx_delete(&whitespace_regex);
	rgx_delete(&lparen_regex);
	rgx_delete(&rparen_regex);
	rgx_delete(&star_regex);
	rgx_delete(&bar_regex);
	rgx_delete(&plus_regex);
	rgx_delete(&char_set_regex);
	rgx_delete(&digit_set_regex);
	rgx_delete(&whitespace_set_regex);
	rgx_delete(&lparen_regex_esc);
	rgx_delete(&rparen_regex_esc);
	rgx_delete(&star_regex_esc);
	rgx_delete(&bar_regex_esc);
	rgx_delete(&plus_regex_esc);
	rgx_delete(&quote_regex);
	rgx_delete(&quote_set_regex);
}

parse_res_t expression(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] expression\n");
	parse_res_t next = term(lkd, regex); 
	parse_res_t res = conjunction(next.stream, next.regex); 
	if (!res.stream)
	{
		LOG("ERROR %p\n", res.regex);
	}
	return res;
}

parse_res_t term(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] term\n");
	parse_res_t next = factor(lkd, regex); 
	parse_res_t res = concatenation(next.stream, next.regex); 
	return res;
}

parse_res_t factor(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] factor\n");
	parse_res_t op_res = operand(lkd, regex); 
	parse_res_t res = length_mod(op_res.stream, op_res.regex); 
	return res;
}

parse_res_t operand(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] operand\n");
	token_node_t* lparen = expect(lkd, Tkn_LParen);
	if (lparen)
	{
		LOG("[PARSER] operand found lparen\n");
		parse_res_t exp = expression(lparen, regex);
		if (exp.stream)
		{
			token_node_t* rparen = expect(exp.stream, Tkn_RParen);
			if (rparen)
			{
				LOG("[PARSER] operand found rparen\n");
				return (parse_res_t) {.regex = exp.regex, .stream = rparen};
			}
			else 
			{
				LOG("[PARSER] missing RPAREN\n");
				return (parse_res_t) {.regex = exp.regex, .stream = NULL};
			}
		}
		else 
		{
			LOG("[PARSER] invalid EXPRESSION\n");
			return (parse_res_t) {.regex = regex, .stream = NULL};
		}
	}
	else 
	{
		LOG("[PARSER] operand expects character\n");
		token_node_t* character = expect(lkd, Tkn_Character);
		if (character)
		{
			LOG("[PARSER] operand found character: %c\n", lkd->value.value.character);
			return (parse_res_t) {.stream = character, .regex = rgx_character(lkd->value.value.character)}; 
		}
		else
		{
			LOG("[PARSER] operand expects char set\n");
			token_node_t* char_set = expect(lkd, Tkn_CharSet);
			if (char_set)
			{
				LOG("[PARSER] operand found charset\n");
				return (parse_res_t) {.stream = char_set, .regex = rgx_char_set()};
			}
			else 
			{
				token_node_t* digit_set = expect(lkd, Tkn_DigitSet);
				if (digit_set)
				{
					LOG("[PARSER] operand found digit set\n");
					return (parse_res_t) {.stream = digit_set, .regex = rgx_digit_set()};
				}
				else 
				{
					token_node_t* whitespace_set = expect(lkd, Tkn_WhitespaceSet);
					if (whitespace_set)
					{
						LOG("[PARSER] operand found whitespace set\n");
						return (parse_res_t) {.stream = whitespace_set, .regex = rgx_whitespace_set()};
					}
					else
					{
						token_node_t* quote_set = expect(lkd, Tkn_QuoteSet);
						if (quote_set)
						{
							LOG("[PARSER] operand found quote set\n");
							return (parse_res_t) { .stream = quote_set, .regex = rgx_quote_set() };
						}
					}
				}
			}
		}
	}
	LOG("[PARSER] operand character not found\n")
	return (parse_res_t) {.stream = NULL, .regex = regex};
}

parse_res_t length_mod(token_node_t* lkd, regex_t* inner)
{
	LOG("[PARSER] length modifier\n");
	token_node_t* star = expect(lkd, Tkn_Star);
	if (star)
	{
		LOG("[PARSER] length mod found star\n");
		return (parse_res_t){.stream = star, .regex = rgx_star(inner)};
	}
	else
	{
		token_node_t* plus = expect(lkd, Tkn_Plus);
		if (plus)
		{
			LOG("[PARSER] length mod found plus\n");
			return (parse_res_t) {.stream = plus, .regex = rgx_plus(inner)};
		}
	}
	// empty
	LOG("[PARSER] length mod assumes empty\n");
	return (parse_res_t) {.stream = lkd, .regex = inner};
}

parse_res_t conjunction(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] conjunction\n");
	token_node_t* bar;
	parse_res_t trm = {NULL, NULL};

	regex_t* conj = regex;
	regex_t* a = NULL;
	bar = expect(lkd, Tkn_Bar);
	while (bar) {
		if (bar)
		{
			LOG("[PARSER] conjunction found bar\n");
			trm = term(bar, conj);
			a = trm.regex;
			conj = rgx_union(conj, a);
			bar = expect(trm.stream, Tkn_Bar);
		}
		else 
			bar = NULL;
	} 
	if (trm.stream)
		return (parse_res_t) {.stream = trm.stream, .regex = conj};
	LOG("[PARSER] conjunction assumes empty\n");
	return (parse_res_t) {.stream = lkd, .regex = regex};
}

parse_res_t concatenation(token_node_t* lkd, regex_t* regex)
{
	LOG("[PARSER] concatenation\n");
	parse_res_t fct = factor(lkd, regex);
	parse_res_t fct2 = {NULL, NULL};

	regex_t* concat = regex;
	regex_t* a = fct.regex;
	while (fct.stream)
	{
		if (fct.stream)
		{
			concat = rgx_concat(concat, a);
			fct2 = fct;	
			fct = factor(fct2.stream, fct2.regex);	
			a = fct.regex;
		}
	}
	if (fct2.stream)
		return (parse_res_t) {.stream = fct2.stream, .regex = concat};
	LOG("[PARSER] concatenation assumes empty\n");
	return (parse_res_t){.stream = lkd, .regex = regex};
}

token_node_t* expect(token_node_t* lkd, token_type tkn)
{
	if (lkd == NULL)
		return NULL;
	if (lkd->value.type == tkn) 
	{
		LOG("[PARSER] Expectation passed %d\n", tkn);
		return lkd->next;
	}
	else 						
	{
		LOG("[PARSER] Expectation failed %d\n", tkn);
		return NULL;
	}
}

regex_t* rgx_compile(const char* src)
{
	token_stream_t stream = {NULL, NULL};
	rgx_ts_init(&stream);
	rgx_tokenize(src, &stream);
	parse_res_t res = expression(stream.head, NULL);
	if (!res.stream)
	{
		rgx_ts_delete(&stream);
		rgx_delete(&(res.regex));
		return NULL;
	}
	if (res.stream->value.type == Tkn_EndOfInput)
	{
		rgx_ts_delete(&stream);
		return res.regex;
	}
	else
	{
		rgx_ts_delete(&stream);
		rgx_delete(&(res.regex));
		return NULL;
	}
}

void rgx_print(const token_t* tkn)
{
	if (!tkn) return;
	switch (tkn->type)
	{
	case Tkn_Character:
		printf("[Character, %c]", tkn->value.character);
		break;
	case Tkn_Bar:
		printf("[|]");
		break;
	case Tkn_LParen:
		printf("[(]");
		break;
	case Tkn_RParen:
		printf("[)]");
		break;
	case Tkn_Star:
		printf("[*]");
		break;
	case Tkn_Plus:
		printf("[+]");
		break;
	case Tkn_CharSet:
		printf("[\\c]");
		break;
	case Tkn_DigitSet:
		printf("[\\d]");
		break;
	case Tkn_WhitespaceSet:
		printf("[\\w]");
		break;
	case Tkn_QuoteSet:
		printf("[\\q]");
		break;
	case Tkn_EndOfInput:
		printf("$");
		break;
	}
}

void rgx_print_stream(const token_stream_t* stream)
{
	printf("{");
	token_node_t* it = stream->head;
	while (it != NULL)
	{
		rgx_print(&it->value);
		if (it->next)printf(", ");
		it = it->next;
	}
	printf("}\n");
}

void rgx_print_parse_res(parse_res_t* res)
{
	if (!res->stream)
	{
		printf("Parsing failed\n");
	}
	else 
	{
		printf("Parsing result: [%p,%p]\nNode value: \n", (void*)res->stream->next, (void*)res->stream->prev);
		rgx_print(&res->stream->value);
		printf("\n");
		rgx_print_regex(res->regex, 0);
		printf("\n");
	}
}
