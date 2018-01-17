/**
 * \file wifi.h
 * \brief Defines structures and functions to manipulate wireless interface
 * \author BUGNOT Thibaut
 * \date 17 january 2018
 * */

#ifndef WIFI_H
#define WIFI_H

#include <net/if.h>

#include "interface.h"
#include "linuxlist.h"
#include "mem.h"
#include "nl80211.h"
#include "util.h"

#define SIZE_SOCKET 8192
#define FLAGS 768 //?

/**
 * \struct wifi_nlstate
 * \brief Group netlink socket with param needed to use it
 * */
struct wifi_nlstate{
	struct nl_sock* sock;/*!< The netlink socket*/
	int nl_id;/*!< The netlink identifier*/
};



 /**
  * \fn int error_cb(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg)
  * \brief Function executed when an error netlink message is catch
  * \param nla A reference on the socket
  * \param err The error netlink message
  * \param arg Must reference an integer which will be set to the error code given by the message
  * */
int error_cb(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
 /**
  * \fn int finish_cb(struct nl_msg *msg, void *arg)
  * \brief Function executed when a finish netlink message is catch
  * \param msg The netlink message
  * \param arg Must reference an integer which will be set to 0
  * */
int finish_cb(struct nl_msg *msg, void *arg);
 /**
  * \fn int ack_cb(struct nl_msg *msg, void *arg)
  * \brief Function executed when a ack netlink message is catch
  * \param msg The netlink message
  * \param arg Must reference an integer which will be set to 0
  * */
int ack_cb(struct nl_msg *msg, void *arg);
/**
 * \fn int phy_handler(struct nl_msg *msg, void *arg)
 * \brief get from an netlink message information about wireless physical device and add it to a list
 * \param msg The netlink message
 * \param arg Must reference a list if wireless physical devices (struct wifi_wiphy) or en empty list
 * */
int phy_handler(struct nl_msg *msg, void *arg);
/**
 * \fn int if_handler(struct nl_msg *msg, void *arg)
 * \brief get from an netlink message information about wireless interface and add it to a list
 * \param msg The netlink message
 * \param arg Must reference a list if wireless interface (struct wifi_interface) or en empty list
 * */
int if_handler(struct nl_msg *msg, void *arg);
/**
 * \fn int send_recv_msg(struct wifi_nlstate* nlstate, enum nl80211_commands cmd, int flags, struct list_head* params, nl_recvmsg_msg_cb_t func, void* arg)
 * \brief Send an netlink message to the kernel and wait for answer
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \param cmd The command to give to the message
 * \param flags The flags to give to the message
 * \param params A reference on a list of parameters (struct nlparam) to add to the message. Can be NULL if message need any parameter.
 * \param func The function which will be called to analyse valid answer. Can be NULL if valid answer doesn't need to be analysed. Else function should have two arguments : a struct nl_msg* and a void*, and return an int.
 * \param arg The argument that will be given to the function designed by previous argument when called. Should be NULL if previous argument is NULL.
 * \return A negative integer if an error append, 0 otherwise
 * */
int send_recv_msg(struct wifi_nlstate* nlstate, enum nl80211_commands cmd, int flags, struct list_head* params, nl_recvmsg_msg_cb_t func, void* arg);
/**
 * \fn int send_ifreq(struct ifreq* ifr)
 * \brief Send an interface request
 * \param ifr Interface request to send
 * \return A negative integer if an error appened or 0 otherwise
 * */
int send_ifreq(struct ifreq* ifr);





/** 
 * \fn const char* wifi_geterror(int err)
 * \brief Get an error message by his code
 * \param err The error code returned by a function
 * \return An error message corresponding to the code
 * */
const char* wifi_geterror(int err);
/**
 * \fn int wifi_init_nlstate(struct wifi_nlstate* nlstate)
 * \brief Fill the struct wifi_nlstate with value needed to correctly use other functions
 * \param nlstate A reference on the struct wifi_nlstate to initialize. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_init_nlstate(struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_wiphy(struct list_head* lwp, struct wifi_nlstate* nlstate)
 * \brief Make the list of wireless physical devices 
 * \param lwp A reference on an empty list, which will be filled with the wireless physical devices detected (struct wifi_wiphy). Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_wiphy(struct list_head* lwp, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_interfaces(struct list_head* lif, struct wifi_nlstate* nlstate)
 * \brief Make the list of interfaces
 * \param lif A reference on an empty list, which will be filled with the interfaces detected (struct wifi_interface). Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_interfaces(struct list_head* lif, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Make the list of interfaces which support type indicated by second argument
 * \param if_res A reference on an empty list, which will be filled with the interfaces detected (struct wifi_interface). Can't be NULL
 * \param type The type interfaces should support
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Make the list of wireless physical device wich support type indicated by second argument
 * \param wp_res A reference on an empty list, which will be filled with the wireless physical devices detected (struct wifi_wiphy). Can't be NULL
 * \param type The type wireless physical devices should support
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_interface_info(struct wifi_interface* inf, char* name, struct wifi_nlstate* nlstate)
 * \brief Get information about an interface
 * \param inf A reference on an empty struct wifi_interface, which will be filled with the information got (struct wifi_wiphy). Can't be NULL
 * \param name The name of the interface. Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_interface_info(struct wifi_interface* inf, char* name, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_change_frequency(char* name, int freq, struct wifi_nlstate* nlstate)
 * \brief Change frequency of an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The new frequency
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_change_frequency(char* name, int freq, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_change_type(char* name, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Change type of an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The new type of interface
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_change_type(char* name, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct wifi_nlstate* nlstate)
 * \brief Create an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The type of the new interface
 * \param wiphy The number of wireless physical device on which the interface will be created
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_set_meshid(char* name, char* meshid, struct wifi_nlstate* nlstate)
 * \brief Set mesh id on a mesh interface
 * \param name The name of the interface. Can't be NULL
 * \param meshid The mesh id to set. Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_set_meshid(char* name, char* meshid, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_up_interface(char* name)
 * \brief Up an interface
 * \param name The name of the interface. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_up_interface(char* name);
/**
 * \fn int wifi_down_interface(char* name)
 * \brief Down an interface
 * \param name The name of the interface. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_down_interface(char* name);
#endif /*WIFI_H*/
