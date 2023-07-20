#include "lsn.h"

// lison list driver code
void lsn_lst_init(lison_list_t* list)
{
	list->head = NULL;
	list->tail = NULL;
	LOG("[LIST] List initialised\n");
}

void lsn_lst_delete(lison_list_t* list)
{
	if (!list) return;
	lison_node_t* del = list->head;
	lison_node_t* next = del;
	while (del)
	{
		next = del->next;
		LOG("[LIST] Deleting object (%d)\n", del->value->tag);
		lsn_delete(&del->value);
		free(del);
		del = next;
	}
	list->head = NULL;
	list->tail = NULL;
	LOG("[LIST] Object list deleted\n");
}

void lsn_lst_append(lison_list_t* list, lison_t* object)
{
	lison_node_t* new = (lison_node_t*)malloc(sizeof(lison_node_t));
	new->value = object;
	LOG("[LIST] Appending object (%d)\n", object->tag);
	if (list->head == NULL)
	{
		new->prev = NULL;
		new->next = NULL;
		list->head = new;
		list->tail = new;
	}
	else if (list->head == list->tail)
	{
		new->prev = list->head;
		new->next = NULL;
		list->tail = new;
		list->head->next = new;
	}
	else 
	{
		list->tail->next = new;
		new->prev = list->tail;
		new->next = NULL;
		list->tail = new;
	}
}

// lison AST driver code
// char* as input may change to str_t
lison_t* lsn_string(char* str)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_String;
	res->value.string = (char*)malloc(strlen(str) + 1);
	strcpy(res->value.string, str);
	return res;
}

lison_t* lsn_tag(char* str)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_Tag;
	res->value.tag = (char*)malloc(strlen(str) + 1);
	strcpy(res->value.tag, str);
	return res;
}

lison_t* lsn_string_str(const str_t *str)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_String;
	res->value.string = (char*)malloc(str->len + 1);
	strncpy(res->value.string, str->data, str->len);
	res->value.string[str->len] = 0;
	return res;
}

lison_t* lsn_tag_str(const str_t *str)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_Tag;
	res->value.tag = (char*)malloc(str->len + 1);
	strncpy(res->value.tag, str->data, str->len);
	res->value.tag[str->len] = 0;
	return res;
}

lison_t* lsn_integer(int value)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_Integer;
	res->value.integer = value;
	return res;
}

lison_t* lsn_float(float value)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_Float;
	res->value.lsn_float= value;
	return res;
}

lison_t* lsn_object(lison_list_t list)
{
	lison_t* res = (lison_t*)malloc(sizeof(lison_t));
	res->tag = LSN_Object;
	res->value.object = list;
	return res;
}

void lsn_delete(lison_t** object)
{
	if (!*object || !object) return;
	LOG("[LISON] Deleting object (%d)\n", (*object)->tag);
	switch ((*object)->tag)
	{
	case LSN_Object:
		lsn_lst_delete(&(*object)->value.object);
		break;
	case LSN_String:
		free((*object)->value.string);
		break;
	case LSN_Tag:
		free((*object)->value.tag);
		break;
	default:
		break;
	}
	free(*object);
	*object = NULL;
}

void lsn_print(lison_t* object)
{
	if (!object) return;
	switch(object->tag)
	{
	case LSN_Object:
	{
		printf("(");
		lison_node_t* act = object->value.object.head;
		while (act)
		{
			lsn_print(act->value);
			act = act->next;
		}
		printf(")");
		break;
	}
	case LSN_String:
	{
		printf("'%s'", object->value.string);
		break;
	}
	case LSN_Integer:
	{
		printf("%d", object->value.integer);
		break;
	}
	case LSN_Float:
	{
		printf("%f", object->value.lsn_float);
		break;
	}
	case LSN_Tag:
	{
		printf(":%s", object->value.tag);
		break;
	}
	default:
		LOG("[LISON] Print unreachable\n");
	}
	printf(" ");
}
