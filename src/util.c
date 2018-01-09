#include <stdlib.h>

#include "../include/util.h"

struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int, char* value_str){
	struct nlparam* res = malloc(sizeof(struct nlparam));
	res->attr = attr;
	res->type = type;
	res->value_int = value_int;
	res->value_str = value_str;
	INIT_LIST_HEAD(&res->entry);
	return res;
}

void del_nlparam_list(struct list_head* nlpl){
	struct list_head* pos, *q;
	struct nlparam* tmp;
	list_for_each_safe(pos, q, nlpl){
		tmp = list_entry(pos, struct nlparam, entry);
		list_del(pos);
		free(tmp);
	}
}





struct list_int* new_list_int(){
	struct list_int* i = malloc(sizeof(struct list_int));
	INIT_LIST_HEAD(&i->entry);
	return i;
}



