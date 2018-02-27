/**
 * \file network.h
 * \brief Defines structures relative to networks and fonctions to manipulate this structures
 * \author BUGNOT Thibaut
 * \date 17 january 2018
 * */

#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/types.h>
#include "linuxlist.h"

/**
 * \struct wifi_network
 * \brief Represent a normal wifi network
 * */
struct wifi_network{
	char* ssid;/*!< The SSID of the network*/
	int channel;/*!< The channel on which the network is present*/
	struct list_head entry;/*!< Used to create list of struct wifi_network*/
};


/**
 * \struct wifi_mesh_network
 * \brief Represent a mesh wifi network
 * */
struct wifi_mesh_network{
	char* meshid;/*!< The mesh ID of the network*/
	int channel;/*!< The channel on which the network is present*/
	u_int8_t path_selection_protocol;
	u_int8_t path_selection_metric;
	u_int8_t congestion_control;
	u_int8_t synchronization_method;
	u_int8_t authentication_protocol;
	u_int8_t number_peerings;
	u_int8_t capability;
	struct list_head entry;/*!< Used to create list of struct wifi_mesh_network*/
};




/**
 * \fn struct wifi_network* new_network()
 * \brief Allocate memory for a new struct wifi_network
 * \return A reference to the new struct wifi_network
 * */
struct wifi_network* new_network();
/**
 * \fn void network_set_ssid(struct wifi_network* nw, const char* ssid, int len)
 * \brief Set the ssid of a struct wifi_network
 * \param nw A reference on the struct wifi_network
 * \param ssid A reference on the first character of the ssid. Must be a Array of len characters without null characters
 * \param len The length of the ssid
 * */
void network_set_ssid(struct wifi_network* nw, const char* ssid, int len);
/**
 * \fn void network_set_channel(struct wifi_network* nw, int channel)
 * \brief Set the channel of a struct wifi_network
 * \param nw A reference on the struct wifi_network
 * \param channel The channel to set
 * */
void network_set_channel(struct wifi_network* nw, int channel);
/**
 * \fn void print_network(const struct wifi_network* nw)
 * \brief Print a struct wifi_network (used for test)
 * \param nw A reference on the struct wifi_network to print
 * */
void print_network(const struct wifi_network* nw);
/**
 * \fn void del_network(struct wifi_network* nw)
 * \brief Free memory used by a struct wifi_network
 * \param nw A reference on the struct wifi_network to delete
 * */
void del_network(struct wifi_network* nw);
/**
 * \fn void del_network_list(struct list_head* l)
 * \brief Clear a list of struct wifi_network and free memory used by each struct wifi_network
 * \param l A reference on the list to clear
 * */
void del_network_list(struct list_head* l);




/**
 * \fn struct wifi_mesh_network* new_mesh_network()
 * \brief Allocate memory for a new struct wifi_mesh_network
 * \return A reference to the new struct wifi_network
 * */
struct wifi_mesh_network* new_mesh_network();
/**
 * \fn void mesh_network_set_meshid(struct wifi_mesh_network* mn,const char* name, int len)
 * \brief Set the mesh id of a struct wifi_mesh_network
 * \param mn A reference on the struct wifi_mesh_network
 * \param meshid A reference on the first character of the meshid
 * \param len The length of the meshid
 * */
void mesh_network_set_meshid(struct wifi_mesh_network* mn,const char* meshid, int len);
/**
 * \fn void mesh_network_set_configuration(struct wifi_mesh_network* mn,const u_int8_t* conf)
 * \brief Set the configuration of a struct wifi_mesh_network
 * \param mn A reference on the struct wifi_mesh_network
 * \param conf A reference on an Array of parameter
 * */
void mesh_network_set_configuration(struct wifi_mesh_network* mn,const u_int8_t* conf);
/**
 * \fn void mesh_network_set_channel(struct wifi_mesh_network* mn, int channel)
 * \brief Set the channel of a struct wifi_mesh_network
 * \param mn A reference on the struct wifi_mesh_network
 * \param channel The channel to set
 * */
void mesh_network_set_channel(struct wifi_mesh_network* mn, int channel);
/**
 * \fn void print_mesh_network(const struct wifi_mesh_network* mn)
 * \brief Print a struct wifi_mesh_network (used for test)
 * \param mn A reference on the struct wifi_mesh_network to print
 * */
void print_mesh_network(const struct wifi_mesh_network* mn);
/**
 * \fn void del_mesh_network(struct wifi_mesh_network* mn)
 * \brief Free memory used by a struct wifi_mesh_network
 * \param mn A reference on the struct wifi_mesh_network to delete
 * */
void del_mesh_network(struct wifi_mesh_network* mn);
/**
 * \fn void del_mesh_network_list(struct list_head* l)
 * \brief Clear a list of struct wifi_mesh_network and free memory used by each struct wifi_mesh_network
 * \param l A reference on the list to clear
 * */
void del_mesh_network_list(struct list_head* l);

#endif //NETWORK_H
