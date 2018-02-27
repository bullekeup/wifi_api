/**
 * \file util.h
 * \brief Defines somes structures, functions and values used in the rest of the code
 * \author BUGNOT Thibaut
 * \date 17 january 2018
 * */
#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>
#include "linuxlist.h"
#include "nl80211.h"

//length
#define ETH_ALEN 6

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
	const char* value_str;/*!< Reference a string if type is TYPE_STRING or the beginning of data id type is TYPE_DATA. Unused if type is TYPE_INT*/
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
struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int,const char* value_str);
/**
 * \fn void del_nlparam_list(struct list_head* nlpl)
 * \brief Clear a list of struct nlparam aand free memory used by each struct nlparam
 * \param nlpl A reference on the list
 * */
void del_nlparam_list(struct list_head* nlpl);


/** 
 * \fn const char* wifi_geterror(int err)
 * \brief Get an error message by his code
 * \param err The error code returned by a function
 * \return An error message corresponding to the code
 * */
const char* wifi_geterror(int err);

/**\fn void mac_addr_data_to_str(u_int8_t* str,const char* data)
 * \brief calculate string form of a mac address
 * \param str Must reference a ATH_LEN*3 (18) length array which will be filled with the string mac address
 * \param data A reference on mac address under binary form
 * */
void mac_addr_data_to_str(char* str,const u_int8_t* data);

/**\fn u_int8_t hex_to_dec(char h)
 * \brief return a value associated with an hexadecimal character
 * \param h An hexadecimal character (0-9,a-f-A-F)
 * \return The value associated with argument
 * */
u_int8_t hex_to_dec(char h);

/**\fn int mac_addr_str_to_data(u_int8_t* data,const char* str)
 * \brief Calculate the binary form of a mac address given by a string
 * \param data Must reference a ETH_ALEN (6) length array which will be filled with the binary mac address
 * \param str A reference of a string under form "xx:xx:xx:xx:xx:xx"
 * \return A negative integer if the second argument is malformed, 0 otherwise.
 * */
int mac_addr_str_to_data(u_int8_t* data,const char* str);

#endif //UTIL_H
