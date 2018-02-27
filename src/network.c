#include <stdlib.h>
#include <stdio.h>

#include "../include/util.h"
#include "../include/linuxlist.h"
#include "../include/network.h"


struct wifi_network* new_network(){
	struct wifi_network* res = malloc(sizeof(struct wifi_network));
	res->ssid = NULL;
	res->channel = -1;
	INIT_LIST_HEAD(&res->entry);
	return res;
}

void network_set_ssid(struct wifi_network* nw, const char* ssid, int len){
	if(len==0){
		return;
	}
	int i;
	nw->ssid = malloc(sizeof(char)*(len+1));
	for(i=0;i<len;i++){
		if(ssid[i]=='\0'){/*malformed packet*/
			free(nw->ssid);
			nw->ssid=NULL;
			return;
		}
		nw->ssid[i] = ssid[i];
	}
	nw->ssid[len] = '\0';
}


void network_set_channel(struct wifi_network* nw, int channel){
	nw->channel = channel;
}

void print_network(const struct wifi_network* nw){
	if(nw->ssid!=NULL){
		printf("ssid : %s\n", nw->ssid);
	}else{
		printf("Unknowed ssid");
	}
	if(nw->channel!=-1){
		printf("\tchannel: %i\n", nw->channel);
	}
}

void del_network(struct wifi_network* nw){
	if(nw==NULL){
		return;
	}
	if(nw->ssid!=NULL){
		free(nw->ssid);
	}
	free(nw);
}

void del_network_list(struct list_head* l){
	if(l==NULL){
		return;
	}
	struct list_head* pos, *q;
	struct wifi_network* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_network, entry);
		list_del(pos);
		del_network(tmp);
	}
}






struct wifi_mesh_network* new_mesh_network(){
	struct wifi_mesh_network* mn = malloc(sizeof(struct wifi_mesh_network));
	mn->meshid = NULL;
	mn->channel =-1;
	mn->path_selection_protocol = -1;
	mn->path_selection_metric = -1;
	mn->congestion_control = -1;
	mn->synchronization_method = -1;
	mn->authentication_protocol = -1;
	mn->number_peerings = -1;
	mn->capability = -1;
	INIT_LIST_HEAD(&mn->entry);
	return mn;
}

void mesh_network_set_meshid(struct wifi_mesh_network* mn,const char* meshid, int len){
	int i;
	mn->meshid = malloc(sizeof(char)*(len+1));
	for(i=0;i<len;i++){
		mn->meshid[i] = meshid[i];
	}
	mn->meshid[len] = '\0';
}

void mesh_network_set_configuration(struct wifi_mesh_network* mn,const u_int8_t* conf){
	mn->path_selection_protocol = conf[0];
	mn->path_selection_metric = conf[1];
	mn->congestion_control = conf[2];
	mn->synchronization_method = conf[3];
	mn->authentication_protocol = conf[4];
	mn->number_peerings = (conf[5]>>1)&0x3f;
	mn->capability = conf[6]&0x7f;
}

void mesh_network_set_channel(struct wifi_mesh_network* mn, int channel){
	mn->channel = channel;
}

void print_mesh_network(const struct wifi_mesh_network* mn){
	if(mn->meshid!=NULL){
		printf("meshid : %s\n", mn->meshid);
	}else{
		printf("unknowed meshid\n");
	}if(mn->channel != -1){
		printf("\tchannel: %i\n", mn->channel);
	}
	if(mn->number_peerings != -1){
		printf("\tnumber of peering: %i\n",mn->number_peerings);
	}
}

void del_mesh_network(struct wifi_mesh_network* mn){
	if(mn==NULL){
		return;
	}
	if(mn->meshid!=NULL){
		free(mn->meshid);
	}
	free(mn);
}

void del_mesh_network_list(struct list_head* l){
	if(l==NULL){
		return;
	}
	struct list_head* pos, *q;
	struct wifi_mesh_network* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_mesh_network, entry);
		list_del(pos);
		del_mesh_network(tmp);
	}
}
