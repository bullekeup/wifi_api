#ifndef LIST_H
#define LIST_H

struct list{
	void* element;
	struct list* next;
};

struct list* new_list(); /*Allocate memory for a struct list*/
void add_in_list(struct list* l, void* elt); /*add the element elt in the list l*/
int set_in_list(struct list* l,int r, void* elt); /*replace the elementat rank r by elt*/
void* get_from_list(struct list* l, int r); /*return the element at rank r in list l*/
int size_list(struct list* l);/*return the size of the list l*/
void del_list(struct list* l);/*free recursively the memory used by list l*/

#endif /*LIST_H*/

