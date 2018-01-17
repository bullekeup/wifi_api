/**
 * \file util.h
 * \brief Defines somes structures, functions and values used in the rest of the code
 * \author BUGNOT Thibaut
 * \date 17 january 2018
 * */
#ifndef UTIL_H
#define UTIL_H

#include "linuxlist.h"
#include "mem.h"
#include "nl80211.h"

//type of data
#define TYPE_INT 1
#define TYPE_STRING 2
#define TYPE_DATA 3

//booleans
#define TRUE 1
#define FALSE 0

/**
 * \struct list_int
 * \brief Used to create a list of integer
 * */
struct list_int{
	int i;/*!< Value*/
	struct list_head entry;/*!< Reference next and previous int in the list*/
};

/**
 * \struct nlparam
 * \brief contains parameters about a attribute to add to a netlink message
 * */
struct nlparam{
	enum nl80211_attrs attr;/*!< Name of the attribute*/
	int value_int;/*!< Value of the attribute if type is TYPE_INT or length of the data if type is TYPE_DATA. Unused if type is TYPE_STRING*/
	char* value_str;/*!< Reference a string if type is TYPE_STRING or the beginning of data id type is TYPE_DATA. Unused if type is TYPE_INT*/
	int type;/*!< Type of data to add to the nlmessage*/
	struct list_head entry;/*!< Used to create list of nlparam*/ 
};

/**
 * \fn struct list_int* new_list_int()
 * \brief Allocate memory for a struct list_int
 * \return A reference on the new struct list_int 
 * */
struct list_int* new_list_int();



/**
 * \fn struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int, char* value_str)
 * \brief Allocate memory for a new struct nlparam and initialize it with values given
 * \param attr The netlink attribute
 * \param type The type of data. Can be TYPE_INT, TYPE_STRING or TYPE_DATA
 * \param value_int The value if type is TYPE_INT or the length of the data if type is TYPE_DATA. Should be 0 if type is TYPE_STRING
 * \param value_str A reference on the first character if type is TYPE_STRING or on the beginning of the data Array if type is TYPE_DATA. Should be NULL if type is TYPE_INT
 * \return A reference on the new struct nlparam
 * */
struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int, char* value_str);
/**
 * \fn void del_nlparam_list(struct list_head* nlpl)
 * \brief Clear a list of struct nlparam aand free memory used by each struct nlparam
 * \param nlpl A reference on the list
 * */
void del_nlparam_list(struct list_head* nlpl);

#endif //UTIL_H
