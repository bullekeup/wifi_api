#ifndef MPATH_H
#define MPATH_H

#include "util.h"

struct wifi_mesh_path{
	unsigned char dest [ETH_ALEN];
	unsigned char next_hop [ETH_ALEN];
	int metric;
	struct list_head entry;
};

struct wifi_mesh_path* new_mesh_path();

void print_mesh_path(struct wifi_mesh_path* mp);

void del_mesh_path_list(struct list_head* l);
#endif /*MPATH_H*/
