/**
 * \file interface.h
 * \brief Defines structures relative to interface and fonctions to manipulate this structures
 * \author BUGNOT Thibaut
 * \date 17 january 2018
 * */

#ifndef INTERFACE_H
#define INTERFACE_H

#include "nl80211.h"
#include "linuxlist.h"

#define ETH_ALEN 6


/**
 * \struct wifi_wiphy
 * \brief Represent a wireless physical device
 * */
struct wifi_wiphy{
	int num; /*!<  Number of the wireless physical device*/
	struct list_int* frequencies; /*!< List of frequencies supported*/
	struct list_int* if_types; /*!< List of types supported*/
	struct list_head entry;/*!< Used to create list of struct_wifi*/
};


/**
 * \struct wifi_interface
 * \brief Represent an wireless interface
 * */
struct wifi_interface{
	char* name; /*!< Name of the interface*/
	int wi_phy; /*! Number of the physical wireless device associated*/
	int type; /*!< Type of connection used*/
	int frequency;/*!< Frequency used*/
	int width;/*!< Width of channel*/
	unsigned char mac [ETH_ALEN]; /*!< Mac adress*/
	struct list_head entry;/*!< Used to create list of interface*/
};




/**
 * \fn void print_wi_phy(struct wifi_wiphy* i)
 * \brief print a struct wifi_wiphy (used for test)
 * \param i A reference to the struct wifi_wiphy to print
 * */
void print_wi_phy(struct wifi_wiphy* i);
/**
 * \fn struct wifi_wiphy* new_wi_phy()
 * \brief Allocate memory for a new struct wifi_wiphy and initialize it
 * \return A reference to the new struct wifi_wiphy
 * */
struct wifi_wiphy* new_wi_phy();
/**
 * \fn void del_wiphy(struct wifi_wiphy* i)
 * \brief Free memory used by a struct wifi_wiphy
 * \param i A reference on the struct wifi_wiphy to delete
 * */
void del_wiphy(struct wifi_wiphy* i);
/**
 * \fn struct wifi_wiphy* clone_wiphy(struct wifi_wiphy* i)
 * \brief Create a copy of an existing struct wifi_wiphy totally indepedant in memory
 * \param i A reference on the struct wifi_wiphy to copy
 * \return A reference to the new struct wifi_wiphy
 * */
struct wifi_wiphy* clone_wiphy(struct wifi_wiphy* i);
/**
 * \fn void wiphy_copy(struct wifi_wiphy* dest, struct wifi_wiphy* src)
 * \brief Copy a struct wifi_wiphy in an other struct wifi_wiphy. The copy is totally independant in memory from the original
 * \param dest A reference on the destination of the copy
 * \param src A reference on the source of the copy
 * */
void wiphy_copy(struct wifi_wiphy* dest, struct wifi_wiphy* src);
/**
 * \fn void del_wiphy_list(struct list_head* l)
 * \brief Clear a list of struct wifi_wiphy and free memory used by each struct wifi_wiphy
 * \param l A reference on the list to clear
 * */
void del_wiphy_list(struct list_head* l);





/**
 * \fn void print_if(struct wifi_interface* i)
 * \brief print a struct wifi_interface (used for test)
 * \param i A reference to the struct wifi_interface to print
 * */
void print_if(struct wifi_interface* i);
/**
 * \fn struct wifi_interface* new_if()
 * \brief Allocate memory for a new struct wifi_interface and initialize it
 * \return A reference to the new struct wifi_interface
 * */
struct wifi_interface* new_if();
/**
 * \fn char* get_if_type(enum nl80211_iftype i)
 * \brief Associate a string to each enum nl80211_iftype (used for print)
 * \param i The type
 * \return The string associated with the type
 * */
char* get_if_type(enum nl80211_iftype i);
/**
 * \fn struct wifi_interface* clone_if(struct wifi_interface* i)
 * \brief Create a copy of an existing struct wifi_winterface totally indepedant in memory
 * \param i A reference on the struct wifi_interface to copy
 * \return A reference to the new struct wifi_interface
 * */
struct wifi_interface* clone_if(struct wifi_interface* i);
/**
 * \fn void if_copy(struct wifi_interface* dest, struct wifi_interface* src)
 * \brief Copy a struct wifi_interface in an other struct wifi_interface. The copy is totally independant in memory from the original
 * \param dest A reference on the destination of the copy
 * \param src A reference on the source of the copy
 * */
void if_copy(struct wifi_interface* dest, struct wifi_interface* src);
/**
 * \fn void del_if(struct wifi_interface* i)
 * \brief Free memory used by a struct wifi_interface
 * \param i A reference on the struct wifi_interface to delete
 * */
void del_if(struct wifi_interface* i);
/**
 * \fn void del_if_list(struct list_head* l)
 * \brief Clear a list of struct wifi_interface and free memory used by each struct wifi_interface
 * \param l A reference on the list to clear
 * */
void del_if_list(struct list_head* l);
#endif /*INTERFACE_H*/
