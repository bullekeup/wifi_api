#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <errno.h>

#include "../include/wifi.h"
#include "../include/util.h"
#include "../include/route.h"
#include "../include/mpath.h"


int mpath_handler(struct nl_msg *msg, void *arg){
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tab[NL80211_ATTR_MAX + 1];
	struct list_head* l = arg;
	struct nlattr *mptab[NL80211_MPATH_INFO_MAX + 1];
	struct wifi_mesh_path* mp = new_mesh_path();
	u_int8_t* mac;
	int i;
	
	/*Parse recieved message*/
	nla_parse(tab, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
	if (!tab[NL80211_ATTR_MPATH_INFO]) {
		return NL_SKIP;
	}
	nla_parse_nested(mptab, NL80211_MPATH_INFO_MAX, tab[NL80211_ATTR_MPATH_INFO], NULL);
	
	/*get informations*/
	mac = (u_int8_t*) nla_data(tab[NL80211_ATTR_MAC]);
	for(i=0;i<ETH_ALEN;i++){
		mp->dest[i] = mac[i];
	}
	mac = (u_int8_t*) nla_data(tab[NL80211_ATTR_MPATH_NEXT_HOP]);
	for(i=0;i<ETH_ALEN;i++){
		mp->next_hop[i] = mac[i];
	}
	if (mptab[NL80211_MPATH_INFO_METRIC]){
		mp->metric = nla_get_u32(mptab[NL80211_MPATH_INFO_METRIC]);
	}
	
	/*add to list*/
	list_add(&mp->entry, l);
	return NL_SKIP;
}

int wifi_add_mesh_path(const char* name, const u_int8_t* dest, const u_int8_t* next_hop, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	int res = 0;
	struct nlparam* p;
	int ifindex;
	
	/*check parameters*/
	if(name==NULL || dest==NULL || next_hop==NULL || nlstate==NULL){
		return -EFAULT;
	}
	ifindex = if_nametoindex(name);
	if(ifindex==0){
		return -ENODEV;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, ifindex, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_MAC, TYPE_DATA, ETH_ALEN, (char*)dest);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_MPATH_NEXT_HOP, TYPE_DATA, ETH_ALEN, (char*)dest);
	list_add(&p->entry, &attrs);
	res = send_recv_msg(nlstate, NL80211_CMD_NEW_MPATH, 0, &attrs, NULL, NULL);
	
	/*free memory*/
	del_nlparam_list(&attrs);
	return res;
}


int wifi_get_mesh_path(struct list_head* lmp, const char* name, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	int res = 0;
	struct nlparam* p;
	int ifindex;
	
	/*check parameters*/
	if(lmp==NULL || name==NULL || nlstate==NULL){
		return -EFAULT;
	}
	ifindex = if_nametoindex(name);
	if(ifindex==0){
		return -ENODEV;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, ifindex, NULL);
	list_add(&p->entry, &attrs);
	res = send_recv_msg(nlstate, NL80211_CMD_GET_MPATH, FLAGS, &attrs, mpath_handler, lmp);
	
	/*free memory*/
	del_nlparam_list(&attrs);
	return res;
}

int wifi_del_mesh_path(const char* name, const u_int8_t* dest, struct wifi_nlstate* nlstate){
	LIST_HEAD(attrs);
	int res = 0;
	struct nlparam* p;
	int ifindex;
	
	/*check parameters*/
	if(name==NULL || dest==NULL || nlstate==NULL){
		return -EFAULT;
	}
	ifindex = if_nametoindex(name);
	if(ifindex==0){
		return -ENODEV;
	}
	
	/*create param list and send message*/
	p = new_nlparam(NL80211_ATTR_IFINDEX, TYPE_INT, ifindex, NULL);
	list_add(&p->entry, &attrs);
	p = new_nlparam(NL80211_ATTR_MAC, TYPE_DATA, ETH_ALEN, (char*)dest);
	list_add(&p->entry, &attrs);
	res = send_recv_msg(nlstate, NL80211_CMD_DEL_MPATH, 0, &attrs, NULL, NULL);
	
	/*free memory*/
	del_nlparam_list(&attrs);
	return res;
}



