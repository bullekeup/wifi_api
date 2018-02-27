/**
 * \file mpath.h
 * \brief Defines structure représenting mesh path and function to manipulate this structure
 * \author BUGNOT Thibaut
 * \date 20 february 2018
 * */
#ifndef MPATH_H
#define MPATH_H

#include <sys/types.h>
#include "util.h"

/** \struct wifi_mesh_path
 * \brief Represent a mest path given by destination, next hop and metric.
 * */
struct wifi_mesh_path{
	u_int8_t dest [ETH_ALEN];/*!< The destination mac address of path*/
	u_int8_t next_hop [ETH_ALEN];/*!< The next hop mac address on path*/
	int metric;/*!< The metric (cost) of path*/
	struct list_head entry;/*!< Used to create list of struct wifi_mesh_path*/
};



/**
 * \fn struct wifi_mesh_path* new_mesh_path()
 * \brief allocate memory for a struct wifi_mesh_path and initialise it
 * \return a reference on strcutre created
 * */
struct wifi_mesh_path* new_mesh_path();
/**
 * \fn void print_mesh_path(const struct wifi_mesh_path* mp)
 * \brief print a struct wifi_mesh_path
 * \param mp A reference on the struct wifi_mesh_path to print
 * */
void print_mesh_path(const struct wifi_mesh_path* mp);
/**
 * \fn void del_mesh_path_list(struct list_head* l)
 * \brief clear a list of struct wifi_mesh_path and free memory used by each struct wifi_mesh_path
 * \param l A reference to the list to clear
 * */
void del_mesh_path_list(struct list_head* l);
#endif /*MPATH_H*/
