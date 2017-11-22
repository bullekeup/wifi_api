#ifndef INTERFACE_H
#define INTERFACE_H
#include "linuxlist.h"
#include "mem.h"
#include "nl80211.h"

#define ETH_ALEN 6

struct list_int{
	int i;/*value*/
	struct list_head entry;/*reference next and prev int in the list*/
};

/*represent a wireless physical device*/
struct wifi_wiphy{
	int num; /*Number of the device*/
	struct list_int* frequencies; /*list of frequencies supported*/
	struct list_int* if_types; /*list of types supported*/
	struct list_head entry;/*reference next and prev wiphy in the list*/
};

/*represent an interface*/
struct wifi_interface{
	char* name; /*name of the interface*/
	int wi_phy; /*number of the physical wireless device associated*/
	int type; /*type of connection actually used*/
	int frequency;/*frequency used*/
	int width;/*width of channel*/
	unsigned char mac [ETH_ALEN]; /*mac adress*/
	struct list_head entry;/*reference next and prev interface in the list*/
};

struct list_int* new_list_int();/*Allocate memory for a struct list int*/

void print_wi_phy(struct wifi_wiphy* i);/*print a struct wifi_wiphy (used for test)*/
struct wifi_wiphy* new_wi_phy();/*Allocate memory for a struct wifi_wiphy*/
void del_wiphy(struct wifi_wiphy* i);/*free memory used by a struct wifi_wiphy*/
void del_wiphy_list(struct list_head* l);/*free memory used by a list of wiphy*/

void print_if(struct wifi_interface* i);/*print a struct wifi_interface (used for test)*/
struct wifi_interface* new_if();/*allocate memory for a struct wifi_interface*/
char* get_if_type(enum nl80211_iftype i); /*associate a string to each enum nl80211_iftype (used for print)*/
struct wifi_interface* clone_if(struct wifi_interface* i);/*create a copy of an interface, totally independant in memory*/
void if_copy(struct wifi_interface* dest, struct wifi_interface* src);/*copy an interface*/
void del_if(struct wifi_interface* i);/*free memory used by a struct wifi_interface*/
void del_if_list(struct list_head* l);/*free memory used by a list of interfaces*/
#endif /*INTERFACE_H*/
