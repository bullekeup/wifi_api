//compilation :  gcc wifi.c $(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <net/if.h>

#include <stdio.h>

#include "../include/linuxlist.h"
#include "../include/mem.h"
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
	struct wifi_wiphy* l = arg;//list
	struct wifi_wiphy* inf = NULL;
	struct wifi_wiphy* tmp;
	int err, nif;
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
	list_for_each_entry(tmp, &l->entry, entry){
		if(tmp->num == nif){
			inf = tmp;
		}
	}
	if(inf == NULL){
		inf = new_wi_phy();
		inf->num = nif;
		list_add(&(inf->entry), &(l->entry));
	}
	/*add information : types supported*/
	if (attrs[NL80211_ATTR_SUPPORTED_IFTYPES]) {
		nla_for_each_nested(mode, attrs[NL80211_ATTR_SUPPORTED_IFTYPES], rem_mode){
			struct list_int* iftype= new_list_int();
			iftype->i = nla_type(mode);
			list_add(&(iftype->entry), &(inf->if_types->entry));
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
						struct list_int* frequence = new_list_int();
						frequence->i = nla_get_u32(tab_freq[NL80211_FREQUENCY_ATTR_FREQ]);
						list_add(&(frequence->entry), &(inf->frequencies->entry));
					}
				}
			}
		}
	}
	return NL_SKIP;
}


int if_handler(struct nl_msg *msg, void *arg)
{
	struct wifi_interface* l = arg;//list
	struct wifi_interface* inf = new_if();
	list_add(&(inf->entry), &(l->entry));
	int err, lg, i;
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
	/*Add actual frequency*/
	if(attrs[NL80211_ATTR_WIPHY_FREQ]){
		inf->frequency = nla_get_u32(attrs[NL80211_ATTR_WIPHY_FREQ]);
		inf->width = nla_get_u32(attrs[NL80211_ATTR_CHANNEL_WIDTH]);
	}
	/*Add mac address*/
	if(attrs[NL80211_ATTR_MAC]){
		char* mac = (char*)nla_data(attrs[NL80211_ATTR_MAC]);
		for(i=0;i<ETH_ALEN;i++){
			inf->mac[i] = mac[i];
		}
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


struct wifi_wiphy* get_wi_phy(struct nl_sock* sock, int nl_id){
	struct nl_msg* msg;
	struct nl_cb* cb;
	struct wifi_wiphy* lwp = new_wi_phy();//list
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		del_wiphy_list(lwp);
		return NULL;
	}
	genlmsg_put(msg, 0, 0, nl_id, 0, FLAGS, NL80211_CMD_GET_WIPHY, 0);
	
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		del_wiphy_list(lwp);
		nlmsg_free(msg);
		return NULL;
	}
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, phy_handler, lwp);
	
	/*send message and receive answer*/
	send_recv_msg(sock, msg, cb, nl_id);
	
	/*free memory*/
	nl_cb_put(cb);
	nlmsg_free(msg);
	return lwp;
}


struct wifi_interface* wifi_get_interfaces(struct nl_sock* sock, int nl_id){
	struct nl_msg* msg;
	struct nl_cb* cb;
	struct wifi_interface* lif = new_if();//list
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		del_if_list(lif);
		return NULL;
	}
	genlmsg_put(msg, 0, 0, nl_id, 0, FLAGS, NL80211_CMD_GET_INTERFACE, 0);
	
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		del_if_list(lif);
		nlmsg_free(msg);
		return NULL;
	}
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, if_handler, lif);
	
	/*send message and receive answer*/
	send_recv_msg(sock, msg, cb, nl_id);
	
	/*free memory*/
	nl_cb_put(cb);
	nlmsg_free(msg);
	return lif;
}

struct wifi_interface* wifi_get_mesh_interfaces(struct nl_sock* sock, int nl_id){
	struct wifi_interface* list_if;//list
	struct wifi_wiphy* list_wiphy;//list
	struct wifi_interface* mesh_if;//list
	struct wifi_interface* inf;
	struct wifi_wiphy* wi_phy;
	int mesh_supported;
	int number_wiphy;
	struct list_int* type;
	/*initialise list*/
	list_if = wifi_get_interfaces(sock, nl_id);
	if(list_if == NULL){
		return NULL;
	}
	list_wiphy = get_wi_phy(sock, nl_id);
	if(list_wiphy==NULL){
		del_if_list(list_if);
		return NULL;
	}
	mesh_if = new_if();
	/*select interfaces associated with a physical device supporting mesh*/
	list_for_each_entry(inf, &list_if->entry, entry){
		mesh_supported = 0;
		number_wiphy = inf->wi_phy;
		list_for_each_entry(wi_phy, &list_wiphy->entry, entry){
			if(number_wiphy == wi_phy->num){
				list_for_each_entry(type, &wi_phy->if_types->entry, entry){
					if(type->i == NL80211_IFTYPE_MESH_POINT){
						mesh_supported = 1;
					}
				}
			}
		}
		if(mesh_supported){
			list_add(&clone_if(inf)->entry, &mesh_if->entry);
		}
	}
	/*free memory*/
	del_wiphy_list(list_wiphy);
	del_if_list(list_if);
	return mesh_if;
}

int wifi_get_interface_info(struct wifi_interface* inf, struct nl_sock* sock, int nl_id, char* name){
	struct nl_msg* msg;
	struct nl_cb* cb;
	struct wifi_interface* lif = new_if();//list
	struct wifi_interface* i;
	int res = 0;
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		return -1;
	}
	genlmsg_put(msg, 0, 0, nl_id, 0, 0, NL80211_CMD_GET_INTERFACE, 0);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, if_nametoindex(name));
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		nlmsg_free(msg);
		return -2;
	}
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, if_handler, lif);
	
	/*send message and receive answer*/
	send_recv_msg(sock, msg, cb, nl_id);
	
	/*get interface and copy it to inf*/
	i = list_first_entry_or_null(&lif->entry, struct wifi_interface, entry);
	if(i == NULL){
		/*name probably don't correspond to any interfaces*/
		res = -3;
	}else{
		if_copy(inf, i);
	}
	
	/*free memory*/
	nl_cb_put(cb);
	nlmsg_free(msg);
	del_if_list(lif);
	
	return res;
	
}
