/**
 * \file route.h
 * \brief Defines functions to manipulate mesh path.
 * \author BUGNOT Thibaut
 * \date 20 february 2018
 * */
#ifndef ROUTE_H
#define ROUTE_H

#include <netlink/attr.h>
#include "wifi.h"

/**
 * \fn int mpath_handler(struct nl_msg *msg, void *arg);
 * \brief get from an netlink message information about mesh path
 * \param msg The netlink message
 * \param arg Must reference a list of mesh path (struct wifi_mesh_path) or an empty list
 * */
int mpath_handler(struct nl_msg *msg, void *arg);
/**
 * \fn int wifi_add_mesh_path(char* name, u_int8_t* dest, u_int8_t* next_hop, struct wifi_nlstate* nlstate)
 * \brief add a new mesh path in mesh table of an interface
 * \param name The name of the interface. Can't be NULL.
 * \param dest The destination mac address under 48 bits form. Can't be NULL.
 * \param next_hop The next hop mac address under 48 bits form. Can't be NULL.
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error happened, 0 otherwise. 
 * */
int wifi_add_mesh_path(char* name, u_int8_t* dest, u_int8_t* next_hop, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_get_mesh_path(struct list_head* lmp, char* name, struct wifi_nlstate* nlstate)
 * \brief Create a list of all mesh path known by an interface
 * \param  A reference on an empty list, which will be filled with the mesh path (struct wifi_mesh_path). Can't be NULL.
 * \param name The name of the interface. Can't be NULL.
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error happened, 0 otherwise
 * */
int wifi_get_mesh_path(struct list_head* lmp, char* name, struct wifi_nlstate* nlstate);
/**
 * \fn int wifi_del_mesh_path(char* name, u_int8_t* dest, struct wifi_nlstate* nlstate)
 * \brief delete a mesh path from the mesh table of an interface.
 * \param name The name of the interface.
 * \param dest The destination mac address of the path to delete.
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error happened, 0 otherwise
 * */
int wifi_del_mesh_path(char* name, u_int8_t* dest, struct wifi_nlstate* nlstate);

#endif /*ROUTE_H*/
