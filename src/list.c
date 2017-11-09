#include <stdlib.h>

#include "../include/list.h"

struct list* new_list(){
	struct list* l = (struct list*)malloc(sizeof(struct list));
	l->element = NULL;
	l->next = NULL;
	return l;
}

void add_in_list(struct list* l, void* elt){
	if(l->element == NULL){
		l->element = elt;
	}else{
		if(l->next == NULL){
			l->next = new_list();
		}
		add_in_list(l->next, elt);
	}
}

int set_in_list(struct list* l,int r, void* elt){
	if(r < 0){
		return -1;
	}
	if(r == 0){
		l->element = elt;
		return 1;
	}else{
		if(l->next == NULL){
			return -2;
		}else{
			return set_in_list(l->next, r-1, elt);
		}
	}
}

void* get_from_list(struct list* l, int r){
	if(r == 0){
		return l->element;
	}else{
		if(l->next == NULL || r < 0){
			return NULL;
		}else{
			return get_from_list(l->next, r-1);
		}
	}
}

int size_list(struct list* l){
	if(l->element == NULL && l->next == NULL){
		return 0;
	}else if(l->next == NULL){
		return 1;
	}else{
		return size_list(l->next)+1;
	}
}

void del_list(struct list* l){
	if(l->element != NULL){
		free(l->element);
	}
	if(l->next != NULL){
		del_list(l->next);
	}
	free(l);
}
