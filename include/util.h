#ifndef UTIL_H
#define UTIL_H

#include "linuxlist.h"
#include "mem.h"
#include "nl80211.h"

//type of data
#define TYPE_INT 1
#define TYPE_STRING 2
#define TYPE_DATA 3

//booleans
#define TRUE 1
#define FALSE 0

struct list_int{
	int i;/*value*/
	struct list_head entry;/*reference next and prev int in the list*/
};

struct nlparam{
	enum nl80211_attrs attr;
	int value_int;
	char* value_str;
	int type;
	struct list_head entry;
};

struct list_int* new_list_int();/*Allocate memory for a struct list int*/

struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int, char* value_str);
void del_nlparam_list(struct list_head* nlpl);

#endif //UTIL_H
