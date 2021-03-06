#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/util.h"
#include "../include/interface.h"
#include "../include/nl80211.h"
#include "../include/linuxlist.h"
#include "../include/mem.h"



void print_wi_phy(const struct wifi_wiphy* i){
	struct list_int* tmp;
	printf("interface phy#%i\nfrequencies :\n",i->num);
	list_for_each_entry(tmp, &i->frequencies->entry, entry){
		printf("\t %i MHz\n", tmp->i);
	}
	printf("types supporté: \n");
	list_for_each_entry(tmp, &i->if_types->entry, entry){
		printf("\ttype : %s\n", get_if_type(tmp->i));
	}
}

struct wifi_wiphy* new_wi_phy(){
	struct wifi_wiphy* i = malloc(sizeof(struct wifi_wiphy));
	INIT_LIST_HEAD(&i->entry);
	i->frequencies = new_list_int();
	i->if_types = new_list_int();
	return i;
}

struct wifi_wiphy* clone_wiphy(const struct wifi_wiphy* i){
	struct wifi_wiphy* res = new_wi_phy();
	wiphy_copy(res,i);
	return res;
}

void wiphy_copy(struct wifi_wiphy* dest, const struct wifi_wiphy* src){
	struct list_int* l_int;
	struct list_int* nl_int;
	dest->num = src->num;
	list_for_each_entry(l_int, &(src->frequencies->entry), entry){
		nl_int = new_list_int();
		nl_int->i = l_int->i;
		list_add(&(nl_int->entry), &(dest->frequencies->entry));
	}
	list_for_each_entry(l_int, &(src->if_types->entry), entry){
		nl_int = new_list_int();
		nl_int->i = l_int->i;
		list_add(&(nl_int->entry), &(dest->if_types->entry));
	}
}

void del_wiphy(struct wifi_wiphy* i){
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

void del_wiphy_list(struct list_head* l){
	struct list_head* pos, *q;
	struct wifi_wiphy* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_wiphy, entry);
		list_del(pos);
		del_wiphy(tmp);
	}
}






void print_if(const struct wifi_interface* i){
	int j;
	printf("phy#%i : %s:\n\ttype : %s\n\tmac : ",i->wi_phy, i->name, get_if_type(i->type));
	for(j=0;j<ETH_ALEN;j++){
		printf("%02x", i->mac[j]);
		if(j==ETH_ALEN-1){
			printf("\n");
		}else{
			printf(":");
		}
	}
	
	if(i->frequency >0){
		printf("\tfrequency : %i\n", i->frequency);
	}
}

struct wifi_interface* new_if(){
	struct wifi_interface* i;
	i = malloc(sizeof(struct wifi_interface));
	INIT_LIST_HEAD(&i->entry);
	i->frequency = -1;
	i->width = -1;
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

struct wifi_interface* clone_if(const struct wifi_interface* i){
	struct wifi_interface* res = malloc(sizeof(struct wifi_interface));
	if_copy(res,i);
	return res;
}

void if_copy(struct wifi_interface* dest, const struct wifi_interface* src){
	int size_name = strlen(src->name);
	int j;
	dest->name = malloc(sizeof(char)*(size_name+1));
	strncpy(dest->name, src->name, size_name+1);
	dest->name[size_name] = '\0';
	dest->wi_phy = src->wi_phy;
	dest->type = src->type;
	INIT_LIST_HEAD(&dest->entry);
	dest->frequency = src->frequency;
	dest->width = src->width;
	for(j=0;j<ETH_ALEN;j++){
		dest->mac[j] = src->mac[j];
	}
}

void del_if(struct wifi_interface* i){
	if(i->name != NULL){
		free(i->name);
	}
	free(i);
}

void del_if_list(struct list_head* l){
	struct list_head* pos, *q;
	struct wifi_interface* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_interface, entry);
		list_del(pos);
		del_if(tmp);
	}
}
