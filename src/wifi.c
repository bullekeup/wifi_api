//compilation :  gcc wifi.c $(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <sys/ioctl.h>

#include <net/if.h>

#include <stdio.h>
#include <errno.h>

#include "../include/util.h"
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
	struct list_head* l = arg;//list
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
	list_for_each_entry(tmp, l, entry){
		if(tmp->num == nif){
			inf = tmp;
		}
	}
	if(inf == NULL){
		inf = new_wi_phy();
		inf->num = nif;
		list_add(&(inf->entry), l);
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
	struct list_head* l = arg;
	struct wifi_interface* inf = new_if();
	list_add(&(inf->entry), l);
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


/** 
 * \fn const char* wifi_geterror(int err)
 * \brief Get an error message by his code
 * \param err The error code returned by a function
 * \return An error message corresponding to the code
 * 
 * */
const char* wifi_geterror(int err){
	if(err<0){
		err = -err;
	}
	if(err==199){
		return "pcap error";
	}
	if(err<200){
		return strerror(err);
	}else{
		return nl_geterror(err-200);
	}
}


/**
 * \fn int wifi_init_nlstate(struct wifi_nlstate* nlstate)
 * \brief Fill the struct wifi_nlstate with value needed to correctly use other functions
 * \param nlstate A reference on the struct wifi_nlstate to initialize. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_init_nlstate(struct wifi_nlstate* nlstate){
	int err;
	if(nlstate == NULL){
		return -EFAULT;
	}
	
	/*init socket*/
	nlstate->sock = nl_socket_alloc();
	if(nlstate->sock == NULL){
		return -ENOMEM;
	}
	nl_socket_set_buffer_size(nlstate->sock, SIZE_SOCKET, SIZE_SOCKET);
	err = nl_connect(nlstate->sock, NETLINK_GENERIC);
	if(err<0){
		goto errout;
	}
	
	/*resolve netlink identifier*/
	err = genl_ctrl_resolve(nlstate->sock, "nl80211");
	if(err<0){
		goto errout;
	}
	nlstate->nl_id = err;
	return 0;
	
	/*error out*/
	errout:
	nl_socket_free(nlstate->sock);
	nlstate->sock = NULL;
	return err-200;
}

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
int send_recv_msg(struct wifi_nlstate* nlstate, enum nl80211_commands cmd, int flags, struct list_head* params, nl_recvmsg_msg_cb_t func, void* arg){
	struct nl_msg* msg;
	struct nl_cb* cb;
	int err;
	struct nlparam* nlp;
	if(nlstate==NULL){
		return -EFAULT;
	}
	
	/*Create message*/
	msg = nlmsg_alloc();
	if (msg == NULL) {
		return -ENOMEM;
	}
	genlmsg_put(msg, 0, 0, nlstate->nl_id, 0, flags, cmd, 0);
	
	/*Add attrs*/
	if(params!=NULL){
		list_for_each_entry(nlp, params, entry){
			if(nlp->type == TYPE_INT){
				nla_put_u32(msg, nlp->attr, nlp->value_int);
			}if (nlp->type == TYPE_STRING){
				nla_put_string(msg, nlp->attr, nlp->value_str);
			}
		}
	}
	
	/*Create callback*/
	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if(cb == NULL){
		nlmsg_free(msg);
		return -ENOMEM;
	}
	if(func!=NULL){
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, func, arg);
	}
	nl_cb_err(cb, NL_CB_CUSTOM, error_cb, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_cb, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_cb, &err);
	
	/*Send message*/
	err = nl_send_auto(nlstate->sock, msg);
	if(err < 0){
		return err-200;
	}
	
	/*receive messages*/
	err = 1;
	while(err>0){
		nl_recvmsgs(nlstate->sock, cb);
	}
	if(err<0){
		return err;
	}
	return 0;
}


/**
 * \fn int wifi_get_wiphy(struct list_head* lwp, struct wifi_nlstate* nlstate)
 * \brief Make the list of wireless physical devices 
 * \param lwp A reference on an empty list, which will be filled with the wireless physical devices detected (struct wifi_wiphy). Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_wiphy(struct list_head* lwp, struct wifi_nlstate* nlstate){
	if(lwp==NULL){
		return -EFAULT;
	}
	return send_recv_msg(nlstate, NL80211_CMD_GET_WIPHY, FLAGS, NULL, phy_handler, lwp);
}


/**
 * \fn int wifi_get_interfaces(struct list_head* lif, struct wifi_nlstate* nlstate)
 * \brief Make the list of interfaces
 * \param lif A reference on an empty list, which will be filled with the interfaces detected (struct wifi_interface). Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_interfaces(struct list_head* lif, struct wifi_nlstate* nlstate){
	if(lif==NULL){
		return -EFAULT;
	}
	return send_recv_msg(nlstate, NL80211_CMD_GET_INTERFACE, FLAGS, NULL, if_handler, lif);
}


/**
 * \fn int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Make the list of interfaces which support type indicated by second argument
 * \param if_res A reference on an empty list, which will be filled with the interfaces detected (struct wifi_interface). Can't be NULL
 * \param type The type interfaces should support
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate){
	LIST_HEAD(list_if);
	LIST_HEAD(list_wiphy);
	struct wifi_interface* inf;
	struct wifi_wiphy* wi_phy;
	int type_supported;
	int number_wiphy;
	int err;
	struct list_int* type_ac;
	if(if_res==NULL){
		return -EFAULT;
	}
	
	/*initialise list*/
	err = wifi_get_interfaces(&list_if, nlstate);
	if(err<0){
		return err;
	}
	err = wifi_get_wiphy(&list_wiphy, nlstate);
	if(err<0){
		del_if_list(&list_if);
		return err;
	}
	
	/*select interfaces associated with a physical device supporting mesh*/
	list_for_each_entry(inf, &list_if, entry){
		type_supported = 0;
		number_wiphy = inf->wi_phy;
		list_for_each_entry(wi_phy, &list_wiphy, entry){
			if(number_wiphy == wi_phy->num){
				list_for_each_entry(type_ac, &wi_phy->if_types->entry, entry){
					if(type_ac->i == type){
						type_supported = 1;
					}
				}
			}
		}
		if(type_supported){
			list_add(&clone_if(inf)->entry, if_res);
		}
	}
	
	/*free memory*/
	del_wiphy_list(&list_wiphy);
	del_if_list(&list_if);
	return 0;
}


/**
 * \fn int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Make the list of wireless physical device wich support type indicated by second argument
 * \param wp_res A reference on an empty list, which will be filled with the wireless physical devices detected (struct wifi_wiphy). Can't be NULL
 * \param type The type wireless physical devices should support
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate){
	LIST_HEAD(list_wiphy);
	struct wifi_wiphy* wi_phy;
	struct list_int* type_ac;
	int type_supported;
	int err;
	if(wp_res==NULL){
		return -EFAULT;
	}
	
	/*initialise list*/
	err = wifi_get_wiphy(&list_wiphy, nlstate);
	if(err<0){
		return err;
	}
	
	/*select wiphy supporting type*/
	list_for_each_entry(wi_phy, &list_wiphy, entry){
		type_supported = 0;
		list_for_each_entry(type_ac, &wi_phy->if_types->entry, entry){
			if(type_ac->i == type){
				type_supported = 1;
			}
		}
		if(type_supported){
			list_add(&clone_wiphy(wi_phy)->entry, wp_res);
		}
	}
	
	/*free memory*/
	del_wiphy_list(&list_wiphy);
	return 0;
}


/**
 * \fn int wifi_get_interface_info(struct wifi_interface* inf, char* name, struct wifi_nlstate* nlstate)
 * \brief Get information about an interface
 * \param inf A reference on an empty struct wifi_interface, which will be filled with the information got (struct wifi_wiphy). Can't be NULL
 * \param name The name of the interface. Can't be NULL
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_get_interface_info(struct wifi_interface* inf, char* name, struct wifi_nlstate* nlstate){
	LIST_HEAD(lif);
	LIST_HEAD(attrs);
	struct wifi_interface* i;
	int res = 0;
	struct nlparam* p;
	if(inf==NULL || name==NULL){
		return -EFAULT;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, if_nametoindex(name), NULL);
	list_add(&p->entry, &attrs);
	res = send_recv_msg(nlstate, NL80211_CMD_GET_INTERFACE, 0, &attrs, if_handler, &lif);
	if(res<0){
		goto out;
	}
	
	/*get interface and copy it to inf*/
	i = list_first_entry_or_null(&lif, struct wifi_interface, entry);
	if(i == NULL){//name doesn't correspond to any interface
		res = -ENODEV;
	}else{
		if_copy(inf, i);
	}
	
	/*free memory*/
	out:
	del_if_list(&lif);
	del_nlparam_list(&attrs);
	return res;
	
}


/**
 * \fn int wifi_change_frequency(char* name, int freq, struct wifi_nlstate* nlstate)
 * \brief Change frequency of an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The new frequency
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_change_frequency(char* name, int freq, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	struct nlparam* p;
	int ifindex;
	int err;
	if(name==NULL){
		return -EFAULT;
	}
	
	/*find interface index*/
	ifindex = if_nametoindex(name);
	if(ifindex == 0){
		return -ENODEV;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, ifindex, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_WIPHY_FREQ, TYPE_INT, freq, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_WIPHY_CHANNEL_TYPE, TYPE_INT, NL80211_CHAN_NO_HT, NULL);
	list_add(&p->entry, &attrs);
	err = send_recv_msg(nlstate, NL80211_CMD_SET_WIPHY, 0, &attrs, NULL, NULL);
		
	/*free memory*/
	del_nlparam_list(&attrs);
	return err;
}


/**
 * \fn int wifi_change_type(char* name, enum nl80211_iftype type, struct wifi_nlstate* nlstate)
 * \brief Change type of an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The new type of interface
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_change_type(char* name, enum nl80211_iftype type, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	struct nlparam* p;
	int ifindex = 0;
	int err;
	if(name==NULL){
		return -EFAULT;
	}
	
	/*find interface index*/
	ifindex = if_nametoindex(name);
	if(ifindex == 0){
		return -ENODEV;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, ifindex, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_IFTYPE, TYPE_INT, type, NULL);
	list_add(&p->entry, &attrs);
	err = send_recv_msg(nlstate, NL80211_CMD_SET_INTERFACE, 0, &attrs, NULL, NULL);
	
	/*free memory*/
	del_nlparam_list(&attrs);
	return err;
}


/**
 * \fn int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct wifi_nlstate* nlstate)
 * \brief Create an interface
 * \param name The name of the interface. Can't be NULL
 * \param freq The type of the new interface
 * \param wiphy The number of wireless physical device on which the interface will be created
 * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	struct nlparam* p;
	int err;
	if(name==NULL){
		return -EFAULT;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_WIPHY, TYPE_INT, wiphy, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_IFNAME, TYPE_STRING, 0, name);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_IFTYPE, TYPE_INT, type, NULL);
	list_add(&p->entry, &attrs);
	err = send_recv_msg(nlstate, NL80211_CMD_SET_INTERFACE, 0, &attrs, NULL, NULL);
	
	/*free memory*/
	del_nlparam_list(&attrs);
	return err;
}


/**
 * \fn int send_ifreq(struct ifreq* ifr)
 * */
int send_ifreq(struct ifreq* ifr){
	int sock;
	int err;
	
	/*create socket*/
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0){
		return -1;
	}
	
	/*send ifreq*/
	err = ioctl(sock, SIOCSIFFLAGS, ifr);
	if(err<0){
		return -errno;
	}
	return 0;
}


/**
 * \fn int wifi_up_interface(char* name)
 * \brief Up an interface
 * \param name The name of the interface. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_up_interface(char* name){
	struct ifreq ifr;
	int err;
	if(name==NULL){
		return -EFAULT;
	}
	
	/*init ifreq*/
	memset(&ifr, 0, sizeof ifr);
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_flags |= IFF_UP;
	
	/*up interface*/
	err = send_ifreq(&ifr);
	return err;
}



/**
 * \fn int wifi_down_interface(char* name)
 * \brief Down an interface
 * \param name The name of the interface. Can't be NULL
 * \return A negative integer if an error append, 0 otherwise
 * */
int wifi_down_interface(char* name){
	struct ifreq ifr;
	int err;
	if(name==NULL){
		return -EFAULT;
	}
	
	/*init ifreq*/
	memset(&ifr, 0, sizeof ifr);
	strncpy(ifr.ifr_name, name, IFNAMSIZ);
	ifr.ifr_flags &= ~IFF_UP;
	
	/*up interface*/
	err = send_ifreq(&ifr);
	return err;
}
