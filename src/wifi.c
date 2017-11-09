//compilation :  gcc wifi.c $(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <stdio.h>

#include "../include/list.h"
#include "../include/interface.h"
#include "../include/nl80211.h"
#include "../include/wifi.h"

int error_cb(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg){
	int* p = arg;
	*p = err->error;
	return NL_STOP;
}


int finish_cb(struct nl_msg *msg, void *arg){
	int* p = arg;
	*p = 0;
	return NL_SKIP;
}


int ack_cb(struct nl_msg *msg, void *arg){
	int* p = arg;
	*p = 0;
	return NL_STOP;
}


int phy_handler(struct nl_msg *msg, void *arg){
	struct list* l = arg;
	struct wiphy* inf = NULL;
	int err, nif, j;
	struct genlmsghdr* msg_pl = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr* attrs [NL80211_ATTR_MAX + 1];
	struct nlattr* tab_bands [NL80211_BAND_ATTR_MAX + 1];
	struct nlattr* tab_freq [NL80211_FREQUENCY_ATTR_MAX + 1];
	struct nlattr* mode, *band, *freq;
	int rem_mode, rem_band, rem_freq;
	/*Creating index of attributes*/
	err = nla_parse(attrs, NL80211_ATTR_MAX, genlmsg_attrdata(msg_pl, 0), genlmsg_attrlen(msg_pl, 0), NULL);
	if(err < 0){
		return NL_STOP;
	}
	/*Search of interface in the list*/
	nif = nla_get_u32(attrs[NL80211_ATTR_WIPHY]);
	for(j=0;j<size_list(l);j++){
		struct wiphy* i = get_from_list(l, j);
		if(i->num == nif){
			inf = i;
		}
	}
	if(inf == NULL){
		inf = new_wi_phy();
		inf->num = nif;
		add_in_list(l,inf);
	}
	/*add information : types supported*/
	if (attrs[NL80211_ATTR_SUPPORTED_IFTYPES]) {
		nla_for_each_nested(mode, attrs[NL80211_ATTR_SUPPORTED_IFTYPES], rem_mode){
			int* iftype= malloc(sizeof(int));
			*iftype = nla_type(mode);
			add_in_list(inf->if_types, iftype);
		}
	}
	/*add information : frequences supported (useful?)*/
	if (attrs[NL80211_ATTR_WIPHY_BANDS]){
		nla_for_each_nested(band, attrs[NL80211_ATTR_WIPHY_BANDS], rem_band){
			nla_parse(tab_bands, NL80211_BAND_ATTR_MAX, nla_data(band),nla_len(band), NULL);
			if(tab_bands[NL80211_BAND_ATTR_FREQS]){
				nla_for_each_nested(freq, tab_bands[NL80211_BAND_ATTR_FREQS], rem_freq){
					nla_parse(tab_freq, NL80211_FREQUENCY_ATTR_MAX, nla_data(freq),nla_len(freq), NULL);
					if (tab_freq[NL80211_FREQUENCY_ATTR_FREQ]){
						int* frequence = malloc(sizeof(int));
						*frequence = nla_get_u32(tab_freq[NL80211_FREQUENCY_ATTR_FREQ]);
						add_in_list(inf->frequencies, frequence);
					}
				}
			}
		}
	}
	return NL_SKIP;
}


int if_handler(struct nl_msg *msg, void *arg)
{
	struct list* l = arg;
	struct interface* inf = malloc(sizeof(struct interface));
	add_in_list(l, inf);
	int err, lg;
	struct genlmsghdr* msg_pl = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr* attrs [NL80211_ATTR_MAX + 1];
	char* name;
	/*Creating index of attributes*/
	err = nla_parse(attrs, NL80211_ATTR_MAX, genlmsg_attrdata(msg_pl, 0), genlmsg_attrlen(msg_pl, 0), NULL);
	if(err < 0){
		return NL_STOP;
	}
	/*add the physical device associated*/
	inf->wi_phy = nla_get_u32(attrs[NL80211_ATTR_WIPHY]);
	/*Add name of the interface*/
	if(attrs[NL80211_ATTR_IFNAME]){
		name = nla_get_string(attrs[NL80211_ATTR_IFNAME]);
		lg = strlen(name);
		inf->name = malloc(sizeof(char)*(lg + 1));
		strncpy(inf->name, name, lg+1);
		inf->name[lg] = '\0';
	}
	/*Add type actually used*/
	if(attrs[NL80211_ATTR_IFTYPE]){
		inf->type = nla_get_u32(attrs[NL80211_ATTR_IFTYPE]);
	}
	
	return NL_SKIP;
}


struct nl_sock* create_nl_socket(int* nl_id){
	int err;
	struct nl_sock* socket;
	socket = nl_socket_alloc();
	if(socket == NULL){
		return NULL;
	}
	nl_socket_set_buffer_size(socket, SIZE_SOCKET, SIZE_SOCKET);
	err = nl_connect(socket, NETLINK_GENERIC);
	if(err){
		return NULL;
	}
	*nl_id = genl_ctrl_resolve(socket, "nl80211");
	if(nl_id < 0){
		return NULL;
	}
	return socket;
}


int send_recv_msg(struct nl_sock* sock, struct nl_msg* msg, struct nl_cb* cb, int nl_id){
	int err;
	/*Send message*/
	err = nl_send_auto(sock, msg);
	if(err < 0){
		return -1;
	}
	
	/*set up callback*/
	nl_cb_err(cb, NL_CB_CUSTOM, error_cb, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_cb, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_cb, &err);
	
	/*receive messages*/
	err = 1;
	while(err>0){
		nl_recvmsgs(sock, cb);
	}
	return err;
}


struct list* get_wi_phy(struct nl_sock* sock, int nl_id){
	struct nl_msg* msg;
	struct nl_cb* cb;
	struct list* l = new_list();
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		printf("erreur dans l'allocation du message\n");
		return NULL;
	}
	genlmsg_put(msg, 0, 0, nl_id, 0, FLAGS, NL80211_CMD_GET_WIPHY, 0);
	
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		printf("erreur dans l'allocation du callback\n");
		nlmsg_free(msg);
		return NULL;
	}
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, phy_handler, l);
	
	/*send message and receive answer*/
	send_recv_msg(sock, msg, cb, nl_id);
	
	/*free memory*/
	nl_cb_put(cb);
	nlmsg_free(msg);
	return l;
}


struct list* wifi_get_interfaces(struct nl_sock* sock, int nl_id){
	struct nl_msg* msg;
	struct nl_cb* cb;
	struct list* l = new_list();
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		printf("erreur dans l'allocation du message\n");
		return NULL;
	}
	genlmsg_put(msg, 0, 0, nl_id, 0, FLAGS, NL80211_CMD_GET_INTERFACE, 0);
	
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		printf("erreur dans l'allocation du callback\n");
		nlmsg_free(msg);
		return NULL;
	}
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, if_handler, l);
	
	/*send message and receive answer*/
	send_recv_msg(sock, msg, cb, nl_id);
	
	/*free memory*/
	nl_cb_put(cb);
	nlmsg_free(msg);
	return l;
}

struct list* wifi_get_mesh_interfaces(struct nl_sock* sock, int nl_id){
	struct list* list_if;
	struct list* list_wiphy;
	struct list* mesh_if;
	struct interface* inf;
	struct wiphy* wi_phy;
	int i, j, k;
	int nb_wiphy, nb_if, nb_types;
	int mesh_supported;
	int number_wiphy;
	int* type;
	/*initialise list*/
	list_if = wifi_get_interfaces(sock, nl_id);
	list_wiphy = get_wi_phy(sock, nl_id);
	mesh_if = new_list();
	/*select interfaces associated with a physical device supporting mesh*/
	nb_wiphy = size_list(list_wiphy);
	nb_if = size_list(list_if);
	for(i=0; i<nb_if; i++){
		inf = get_from_list(list_if, i);
		mesh_supported = 0;
		number_wiphy = inf->wi_phy;
		for(j=0; j<nb_wiphy && !mesh_supported; j++){
			wi_phy = get_from_list(list_wiphy, j);
			nb_types = size_list(wi_phy->if_types);
			for(k=0;number_wiphy == wi_phy->num && k<nb_types && !mesh_supported; k++){
				type = get_from_list(wi_phy->if_types, k);
				if(*type == NL80211_IFTYPE_MESH_POINT){
					mesh_supported = 1;
				}
			}
		}
		if(mesh_supported){
			add_in_list(mesh_if, clone_if(inf));
		}
	}
	/*free memory*/
	del_wiphy_list(list_wiphy);
	del_if_list(list_if);
	return mesh_if;
}
