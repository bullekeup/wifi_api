#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/interface.h"
#include "../include/nl80211.h"
#include "../include/linuxlist.h"
#include "../include/mem.h"

struct list_int* new_list_int(){
	struct list_int* i = malloc(sizeof(struct list_int));
	INIT_LIST_HEAD(&i->entry);
	return i;
}




void print_wi_phy(struct wiphy* i){
	struct list_int* tmp;
	printf("interface phy#%i\nfrequencies :\n", i->num);
	list_for_each_entry(tmp, &i->frequencies->entry, entry){
		printf("\t %i MHz\n", tmp->i);
	}
	printf("types supporté: \n");
	list_for_each_entry(tmp, &i->if_types->entry, entry){
		printf("\ttype : %s\n", get_if_type(tmp->i));
	}
}

struct wiphy* new_wi_phy(){
	struct wiphy* i = malloc(sizeof(struct wiphy));
	INIT_LIST_HEAD(&i->entry);
	i->frequencies = new_list_int();
	i->if_types = new_list_int();
	return i;
}

void del_wiphy(struct wiphy* i){
	struct list_head* pos, *q;
	struct list_int* tmp;
	list_for_each_safe(pos, q, &i->frequencies->entry){
		tmp= list_entry(pos, struct list_int, entry);
		list_del(pos);
		free(tmp);
	}
	free(i->frequencies);
	list_for_each_safe(pos, q, &i->if_types->entry){
		tmp = list_entry(pos, struct list_int, entry);
		list_del(pos);
		free(tmp);
	}
	free(i->if_types);
	free(i);
}

void del_wiphy_list(struct wiphy* i){
	struct list_head* pos, *q;
	struct wiphy* tmp;
	list_for_each_safe(pos, q, &i->entry){
		tmp = list_entry(pos, struct wiphy, entry);
		list_del(pos);
		del_wiphy(tmp);
	}
	free(i);
}





void print_if(struct interface* i){
	printf("phy#%i : %s:\n\ttype : %s\n",i->wi_phy, i->name, get_if_type(i->type));
}

struct interface* new_if(){
	struct interface* i;
	i = malloc(sizeof(struct interface));
	INIT_LIST_HEAD(&i->entry);
	return i;
}

char* get_if_type(enum nl80211_iftype i){
	char* names [] = {"Unspecified",
		"AdHoc", "Station", "Access point",
		"AP vlan", "WSD", "monitor",
		"Mesh point", "P2P Client", "P2P Go"
		"P2P Device"};
	return names[i];
}

struct interface* clone_if(struct interface* i){
	int size_name = strlen(i->name);
	struct interface* res = malloc(sizeof(struct interface));
	res->name = malloc(sizeof(char)*(size_name+1));
	strncpy(res->name, i->name, size_name+1);
	res->name[size_name] = '\0';
	res->wi_phy = i->wi_phy;
	res->type = i->type;
	INIT_LIST_HEAD(&res->entry);
	return res;
}

void del_if(struct interface* i){
	if(i->name != NULL){
		free(i->name);
	}
	free(i);
}

void del_if_list(struct interface* i){
	struct list_head* pos, *q;
	struct interface* tmp;
	list_for_each_safe(pos, q, &i->entry){
		tmp = list_entry(pos, struct interface, entry);
		list_del(pos);
		del_if(tmp);
	}
}
