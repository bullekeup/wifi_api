#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <sys/types.h>
#include "linuxlist.h"

struct wifi_network{
	char* ssid;
	int channel;
	struct list_head entry;
};


struct wifi_mesh_network{
	char* name;
	int channel;
	u_int8_t path_selection_protocol;
	u_int8_t path_selection_metric;
	u_int8_t congestion_control;
	u_int8_t synchronization_method;
	u_int8_t authentication_protocol;
	u_int8_t number_peerings;
	u_int8_t capability;
	struct list_head entry;
};


struct wifi_network* new_network();
void network_set_ssid(struct wifi_network* nw, const char* ssid, int len);
void network_set_channel(struct wifi_network* nw, int channel);
void del_network(struct wifi_network* nw);

struct wifi_mesh_network* new_mesh_network();
void mesh_network_set_ssid(struct wifi_mesh_network* mn,const char* name, int len);
void mesh_network_set_configuration(struct wifi_mesh_network* mn,const u_int8_t* conf);
void mesh_network_set_channel(struct wifi_mesh_network* mn, int channel);
void print_mesh_network(struct wifi_mesh_network* mn);
void del_mesh_network(struct wifi_mesh_network* mn);
void del_mesh_network_list(struct list_head* l);

#endif //NETWORK_H
