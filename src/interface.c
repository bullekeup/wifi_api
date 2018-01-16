/**
 * \file interface.c
 * */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../include/util.h"
#include "../include/interface.h"
#include "../include/nl80211.h"
#include "../include/linuxlist.h"
#include "../include/mem.h"


/**
 * \fn void print_wi_phy(struct wifi_wiphy* i)
 * \brief print a struct wifi_wiphy (used for test)
 * \param i A reference to the struct wifi_wiphy to print
 * */
void print_wi_phy(struct wifi_wiphy* i){
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

/**
 * \fn struct wifi_wiphy* new_wi_phy()
 * \brief Allocate memory for a new struct wifi_wiphy and initialize it
 * \return A reference to the new struct wifi_wiphy
 * */
struct wifi_wiphy* new_wi_phy(){
	struct wifi_wiphy* i = malloc(sizeof(struct wifi_wiphy));
	INIT_LIST_HEAD(&i->entry);
	i->frequencies = new_list_int();
	i->if_types = new_list_int();
	return i;
}

/**
 * \fn struct wifi_wiphy* clone_wiphy(struct wifi_wiphy* i)
 * \brief Create a copy of an existing struct wifi_wiphy totally indepedant in memory
 * \param i A reference on the struct wifi_wiphy to copy
 * \return A reference to the new struct wifi_wiphy
 * */
struct wifi_wiphy* clone_wiphy(struct wifi_wiphy* i){
	struct wifi_wiphy* res = new_wi_phy();
	wiphy_copy(res,i);
	return res;
}

/**
 * \fn void wiphy_copy(struct wifi_wiphy* dest, struct wifi_wiphy* src)
 * \brief Copy a struct wifi_wiphy in an other struct wifi_wiphy. The copy is totally independant in memory from the original
 * \param dest A reference on the destination of the copy
 * \param src A reference on the source of the copy
 * */
void wiphy_copy(struct wifi_wiphy* dest, struct wifi_wiphy* src){
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

/**
 * \fn void del_wiphy(struct wifi_wiphy* i)
 * \brief Free memory used by a struct wifi_wiphy
 * \param i A reference on the struct wifi_wiphy to delete
 * */
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

/**
 * \fn void del_wiphy_list(struct list_head* l)
 * \brief Clear a list of struct wifi_wiphy and free memory used by each struct wifi_wiphy
 * \param l A reference on the list to clear
 * */
void del_wiphy_list(struct list_head* l){
	struct list_head* pos, *q;
	struct wifi_wiphy* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_wiphy, entry);
		list_del(pos);
		del_wiphy(tmp);
	}
}





/**
 * \fn void print_if(struct wifi_interface* i)
 * \brief print a struct wifi_interface (used for test)
 * \param i A reference to the struct wifi_interface to print
 * */
void print_if(struct wifi_interface* i){
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

/**
 * \fn struct wifi_interface* new_if()
 * \brief Allocate memory for a new struct wifi_interface and initialize it
 * \return A reference to the new struct wifi_interface
 * */
struct wifi_interface* new_if(){
	struct wifi_interface* i;
	i = malloc(sizeof(struct wifi_interface));
	INIT_LIST_HEAD(&i->entry);
	i->frequency = -1;
	i->width = -1;
	return i;
}

/**
 * \fn char* get_if_type(enum nl80211_iftype i)
 * \brief Associate a string to each enum nl80211_iftype (used for print)
 * \param i The type
 * \return The string associated with the type
 * */
char* get_if_type(enum nl80211_iftype i){
	char* names [] = {"Unspecified",
		"AdHoc", "Station", "Access point",
		"AP vlan", "WSD", "monitor",
		"Mesh point", "P2P Client", "P2P Go"
		"P2P Device"};
	return names[i];
}

/**
 * \fn struct wifi_interface* clone_if(struct wifi_interface* i)
 * \brief Create a copy of an existing struct wifi_winterface totally indepedant in memory
 * \param i A reference on the struct wifi_interface to copy
 * \return A reference to the new struct wifi_interface
 * */
struct wifi_interface* clone_if(struct wifi_interface* i){
	struct wifi_interface* res = malloc(sizeof(struct wifi_interface));
	if_copy(res,i);
	return res;
}

/**
 * \fn void if_copy(struct wifi_interface* dest, struct wifi_interface* src)
 * \brief Copy a struct wifi_interface in an other struct wifi_interface. The copy is totally independant in memory from the original
 * \param dest A reference on the destination of the copy
 * \param src A reference on the source of the copy
 * */
void if_copy(struct wifi_interface* dest, struct wifi_interface* src){
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

/**
 * \fn void del_if(struct wifi_interface* i)
 * \brief Free memory used by a struct wifi_interface
 * \param i A reference on the struct wifi_interface to delete
 * */
void del_if(struct wifi_interface* i){
	if(i->name != NULL){
		free(i->name);
	}
	free(i);
}

/**
 * \fn void del_if_list(struct list_head* l)
 * \brief Clear a list of struct wifi_interface and free memory used by each struct wifi_interface
 * \param l A reference on the list to clear
 * */
void del_if_list(struct list_head* l){
	struct list_head* pos, *q;
	struct wifi_interface* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_interface, entry);
		list_del(pos);
		del_if(tmp);
	}
}
