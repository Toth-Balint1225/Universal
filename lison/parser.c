#include "lsn.h"
#include <rgx.h>


// Token Stream driver
void lsn_ts_init(lsn_token_stream_t* stream)
{
	if (!stream) return;
	if (stream->head)
		lsn_ts_delete(stream);

	// continue with init
	stream->head = (lsn_token_node_t*)malloc(sizeof(lsn_token_node_t));
	stream->head->next = NULL;
	stream->head->prev = NULL;
	stream->head->value.tag = LSN_TKN_EOF;
	stream->tail = stream->head;
}

void lsn_ts_delete(lsn_token_stream_t* stream)
{
	if (stream == NULL) return;
	lsn_token_node_t* del = stream->head;
	lsn_token_node_t* next = del;

	while (del != NULL)
	{
		next = del->next;
		LOG("[LISON STREAM] Deleting token(%d)\n", del->value.tag);
		free(del);
		del = next;
	} 
	stream->head = NULL;
	stream->tail = NULL;
}

void lsn_ts_append(lsn_token_stream_t* stream, lsn_token_t tkn)
{
	lsn_token_node_t* new = 
		(lsn_token_node_t*)malloc(sizeof(lsn_token_node_t));
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

void print_str(const str_t* str)
{
	for (size_t i=0;i<str->len;i++)
		printf("%c",str->data[i]);
}

void lsn_print_token(const lsn_token_t* tkn)
{
	switch (tkn->tag)
	{
	case LSN_TKN_String:
		printf("[String, ");
		print_str(&tkn->value.string);
		printf("]");
		break;
	case LSN_TKN_Integer:
		printf("[Integer, %d]", tkn->value.integer);
		break;
	case LSN_TKN_Float:
		printf("[Float, %f]", tkn->value.lsn_float);
		break;
	case LSN_TKN_Tag:
		printf("[Tag, ");
		print_str(&tkn->value.tag);
		printf("]");
		break;
	case LSN_TKN_LParen:
		printf("[(]");
		break;
	case LSN_TKN_RParen:
		printf("[)]");
		break;
	case LSN_TKN_EOF:
		printf("$");
		break;
	case LSN_TKN_CommStart:
		printf("[(*]");
		break;
	case LSN_TKN_CommEnd:
		printf("[*)]");
		break;
	case LSN_TKN_Whitespace:
		break;
	}
}

void lsn_ts_print(lsn_token_stream_t* stream)
{
	printf("{");
	lsn_token_node_t* it = stream->head;
	while (it != NULL)
	{
		lsn_print_token(&it->value);
		if (it->next)printf(", ");
		it = it->next;
	}
	printf("}\n");
}

// Parser 
void lsn_tokenize(char* src, lsn_token_stream_t* stream)
{
	// list of regexes (the poor C programmer's static map)
	regex_t* regex[LSN_TKN_EOF] = {
		[LSN_TKN_CommStart] = rgx_compile("\\(\\*"),
		[LSN_TKN_CommEnd] = rgx_compile("\\*\\)"),
		[LSN_TKN_LParen] = rgx_compile("\\("),
		[LSN_TKN_RParen] = rgx_compile("\\)"),
		[LSN_TKN_String] = rgx_compile("'(\\c|\\w|\\d)*'"),
		[LSN_TKN_Tag] = rgx_compile(":(\\c|\\d)+"),
		[LSN_TKN_Float] = rgx_compile("\\d+.\\d+"),
		[LSN_TKN_Integer] = rgx_compile("\\d+"),
		[LSN_TKN_Whitespace] = rgx_compile("\\w"),
	};

	/* for (size_t i=0;i<LSN_TKN_EOF;i++) */
	/* { */
	/* 	printf("Regex No. %lu\n", i); */
	/* 	rgx_print_regex(regex[i], 0); */
	/* 	printf("\n\n"); */
	/* } */

	char* pointer = src;
	size_t regex_idx = 0;
	str_t res;
	bool in_comment = false;
	// While there is remaining string
	while (*pointer != 0)
	{
		regex_idx = 0;
		// find the first matching string
		while (regex_idx < LSN_TKN_EOF)
		{
			res = rgx_match(pointer, regex[regex_idx]);
			if (res.len != 0)
				break;
			regex_idx++;
		}
		lsn_token_t token;
		token.tag = regex_idx;
		switch (token.tag) {
		case LSN_TKN_String:
		{
			str_t strval = { .data = res.data + 1, .len = res.len - 2 };
			token.value.string = strval;
			break;
		}
		case LSN_TKN_Float:
			token.value.lsn_float = str_to_float(&res);
			break;
		case LSN_TKN_Integer:
			token.value.integer = str_to_int(&res);
			break;
		case LSN_TKN_Tag:
		{
			str_t tokval = { .data = res.data + 1, .len = res.len - 1 };
			token.value.tag = tokval;
			break;
		}
		case LSN_TKN_CommStart:
			in_comment = true;
			break;
		case LSN_TKN_CommEnd:
			in_comment = false;
			break;
		default:
			break;
		}
		// Comment handling
		if (in_comment)
		{
			pointer = pointer + 1;
			continue;
		}

		// Unrecognisible Token Handling
		if (token.tag == LSN_TKN_EOF)
		{
			LOG("[LSN TOKEN] Unrecognisible token, rem: %s\n", res.data);
			break;
		}

		// Adding the new token to the stream
		if (!(token.tag == LSN_TKN_Whitespace
			  || token.tag == LSN_TKN_CommEnd
			  || token.tag == LSN_TKN_CommStart))
			lsn_ts_append(stream, token);

		// Updating them pointer and continue
		pointer = pointer + res.len;
	}


	// Clearing the regex map
	for (size_t i=0;i<LSN_TKN_EOF;i++)
	{
		rgx_delete(&regex[i]);
	}
}

lsn_parse_res_t lsn_p_object(lsn_token_node_t* lkd)
{
	LOG("[LSN PARSER] Object\n");
	lsn_token_node_t* lparen = lsn_p_expect(lkd, LSN_TKN_LParen);
	if (lparen)
	{
		LOG("[LSN PARSER] Object Found Left Paren\n");
		lsn_parse_res_t list = lsn_p_list(lparen);
		if (list.stream)
		{
			lsn_token_node_t* rparen =
				lsn_p_expect(list.stream, LSN_TKN_RParen);
			if (rparen)
			{
				LOG("[LSN PARSER] Object Found Right Paren\n");
				return (lsn_parse_res_t) { .lison = list.lison, .stream = rparen };
			}
			else
			{
				LOG("[LSN PARSER] Object Compile Error, expected Right Paren\n");
				lsn_delete(&list.lison);
			}
		}
		else
		{
			LOG("[LSN PARSER] Object Compile error, wrong list (unreachable)\n");
		}
	}
	else
	{
		lsn_token_node_t* string = lsn_p_expect(lkd, LSN_TKN_String);
		if (string)
		{
			LOG("[LSN PARSER] Object found string\n");
			return (lsn_parse_res_t) { .lison = lsn_string_str(&lkd->value.value.string), .stream =  string };
		}
		else
		{
			lsn_token_node_t* integer = lsn_p_expect(lkd, LSN_TKN_Integer);
			if (integer)
			{
				LOG("[LSN PARSER] Object found integer\n");
				return (lsn_parse_res_t) { .lison = lsn_integer(lkd->value.value.integer), .stream = integer };
			}
			else
			{
				lsn_token_node_t* floating = lsn_p_expect(lkd, LSN_TKN_Float);
				if (floating)
				{
					LOG("[LSN PARSER] Object found float\n");
					return (lsn_parse_res_t) { .lison = lsn_float(lkd->value.value.lsn_float), .stream = floating };
				}
				else
				{
					lsn_token_node_t* tag = lsn_p_expect(lkd, LSN_TKN_Tag);
					if (tag)
					{
						LOG("[LSN PARSER] Object found tag\n");
						return (lsn_parse_res_t) { .lison = lsn_tag_str(&lkd->value.value.tag), .stream = tag};
					}
				}
			}
		}
		LOG("[LSN PARSER] Object Assumes empty\n");
	}
	return (lsn_parse_res_t) { .lison = NULL, .stream = NULL };
}

lsn_parse_res_t lsn_p_list(lsn_token_node_t* lkd)
{
	LOG("[LSN PARSER] List\n");
	lison_list_t list = {NULL, NULL};
	lsn_lst_init(&list);

	lsn_parse_res_t obj1 = lsn_p_object(lkd);
	lsn_parse_res_t obj2 = {NULL, NULL};

	while (obj1.stream)
	{
		if (obj1.stream)
		{
			lsn_lst_append(&list, obj1.lison);
			obj2 = obj1;
			obj1 = lsn_p_object(obj2.stream);
		}
	}
	if (obj2.stream)
	{
		return (lsn_parse_res_t) {.stream = obj2.stream, .lison = lsn_object(list)};
	}
	
	LOG("[LSN PARSER] List Assumes Empty\n");
	return (lsn_parse_res_t) { .stream = lkd, .lison = lsn_object(list)};
}

lsn_token_node_t* lsn_p_expect(lsn_token_node_t* lkd, lsn_token_tag_t tkn)
{
	if (lkd == NULL)
		return NULL;
	if (lkd->value.tag == tkn) 
	{
		LOG("[LSN PARSER] Expectation passed %d\n", tkn);
		return lkd->next;
	}
	else 						
	{
		LOG("[LSN PARSER] Expectation failed %d\n", tkn);
		return NULL;
	}
}


// API
lison_t* lsn_compile(char* src)
{
	lsn_token_stream_t stream = { .head = NULL, .tail = NULL };
	lsn_ts_init(&stream);
	lsn_tokenize(src, &stream);
	lsn_parse_res_t res = lsn_p_object(stream.head);
	if (res.stream == NULL)
	{
		lsn_delete(&res.lison);
		lsn_ts_delete(&stream);
		return NULL;
	}
	else if (res.stream->value.tag == LSN_TKN_EOF)
	{
		lsn_ts_delete(&stream);
		return res.lison;
	}
	else
	{
		lsn_ts_delete(&stream);
		lsn_delete(&res.lison);
		return NULL;
	}
}
