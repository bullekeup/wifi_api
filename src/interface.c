#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/interface.h"
#include "../include/nl80211.h"


void print_wi_phy(struct wiphy* i){
	int j;
	printf("interface phy#%i\nfrequencies :", i->num);
	for(j=0;i->frequencies!=NULL && j<size_list(i->frequencies); j++){
		int* f = get_from_list(i->frequencies, j);
		printf("\t%i MHz\n", *f);
	}
	printf("types supporté: \n");
	for(j=0;i->if_types!=NULL && j<size_list(i->if_types); j++){
		enum nl80211_iftype* t = get_from_list(i->if_types, j);
		printf("\ttype : %s\n", get_if_type(*t));
	}
}

struct wiphy* new_wi_phy(){
	struct wiphy* i = malloc(sizeof(struct wiphy));
	i->frequencies = new_list();
	i->if_types = new_list();
}

void del_wiphy(struct wiphy* i){
	if(i->frequencies != NULL){
		del_list(i->frequencies);
		i->frequencies = NULL;
	}
	if(i->if_types != NULL){
		del_list(i->if_types);
		i->if_types = NULL;
	}
	free(i);
}

void del_wiphy_list(struct list* l){
	int i;
	int nb = size_list(l);
	struct wiphy* wi_phy;
	for(i=0; i<nb;i++){
		wi_phy = get_from_list(l, i);
		if(wi_phy != NULL){
			del_wiphy(wi_phy);
			set_in_list(l, i, NULL);
		}
	}
	del_list(l);
}





void del_if(struct interface* i){
	if(i->name != NULL){
		free(i->name);
	}
	free(i);
}

void print_if(struct interface* i){
	printf("phy#%i : %s:\n\ttype : %s\n",i->wi_phy, i->name, get_if_type(i->type));
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
	return res;
}

void del_if_list(struct list* l){
	int i;
	int nb = size_list(l);
	struct interface* inf;
	for(i=0; i<nb;i++){
		inf = get_from_list(l, i);
		if(inf != NULL){
			del_if(inf);
			set_in_list(l, i, NULL);
		}
	}
	del_list(l);
}
