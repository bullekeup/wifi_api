#ifndef INTERFACE_H
#define INTERFACE_H
#include "list.h"
#include "nl80211.h"

/*represent a wireless physical device*/
struct wiphy{
	int num; /*Number of the device*/
	struct list* frequencies; /*list of frequencies supported*/
	struct list* if_types; /*list of types supported*/
};

/*represent an interface*/
struct interface{
	char* name; /*name of the interface*/
	int wi_phy; /*number of the physical wireless device associated*/
	int type; /*type of connection actually used*/
};

void print_wi_phy(struct wiphy* i);/*print a struct wiphy (used for test)*/
struct wiphy* new_wi_phy();/*Allocate memory for a struct wiphy*/
void del_wiphy(struct wiphy* i);/*free memory used by a struct wiphy*/
void del_wiphy_list(struct list* l);/*free memory used by a list of wiphy*/

void print_if(struct interface* i);/*print a struct interface (used for test)*/
char* get_if_type(enum nl80211_iftype i); /*associate a string to each enum nl80211_iftype (used for print)*/
struct interface* clone_if(struct interface* i);/*create a copy of an interface, totally independant in memory*/
void del_if(struct interface* i);/*free memory used by a struct interface*/
void del_if_list(struct list* l);/*free memory used by a list of interfaces*/
#endif /*INTERFACE_H*/
