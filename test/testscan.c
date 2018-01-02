#include <stdio.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <net/if.h>

#include "../include/nl80211.h"
#include "../include/network.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/scan2.h"
#include "../include/interface.h"
#include "../include/wifi.h"

#define DEFAULT_MONITOR_INTERFACE "mon0"

int main(int argc, char** argv){
	LIST_HEAD(l_mn);
	LIST_HEAD(l_wp);
	LIST_HEAD(l_if);
	struct wifi_wiphy* wp;
	struct wifi_mesh_network* mn;
	struct wifi_interface* inf;
	struct wifi_interface dev_inf;
	struct nl_sock* socket;
	char* dev;
	int nl_id;
	int num_dev;
	int err;
	/*get interface name*/
	if(argc<2){
		dev = DEFAULT_MONITOR_INTERFACE;
	}else{
		dev = argv[1];
	}
	/*Get param for nl message*/
	socket = create_nl_socket(&nl_id);
	/*Creeate interface if doesn't exist*/
	num_dev = if_nametoindex(dev);
	if(num_dev==0){//interface doesn't exist
		/*find a wiphy wich can be monitor*/
		err = wifi_get_wiphy_supporting_type(&l_wp,NL80211_IFTYPE_MONITOR,socket,nl_id);
		if(err<0){
			printf("error while getting wiphy list supporting monitor\n");
			return err;
		}
		wp = list_first_entry_or_null(&l_wp, struct wifi_wiphy, entry);
		if(wp==NULL){
			printf("error : no interface supporting mesh\n");
			return 0;
		}
		/*creating interface*/
		err = wifi_create_interface(dev, NL80211_IFTYPE_MONITOR, wp->num, socket, nl_id);
		if(err<0){
			printf("error while creating monitor interface\n");
		}
		/*free memory*/
		del_wiphy_list(&l_wp);
	}
	/*Down all interface on same wiphy*/
	err = wifi_get_interfaces(&l_if, socket, nl_id);
	if(err<0){
		printf("error while getting interface list\n");
		return err;
	}
	err = wifi_get_interface_info(&dev_inf, socket, nl_id, dev);
	if(err<0){
		printf("error while getting interface %s\n", dev);
		return err;
	}
	list_for_each_entry(inf, &l_if, entry){
		if(inf->wi_phy == dev_inf.wi_phy){
			err = wifi_down_interface(inf->name);
			if(err<0){
				printf("error while down interface %s\n",inf->name);
				return err;
			}
		}
	}
	/*Up monitor interface*/
	err = wifi_up_interface(dev);
	if(err<0){
		printf("error while up interface %s\n",dev);
		return err;
	}
	/*Perform scan and print result*/
	err = scan_all_frequencies(&l_mn, dev, socket, nl_id);
	if(err<0){
		printf("error while scanning\n");
	}
	list_for_each_entry(mn, &l_mn, entry){
		print_mesh_network(mn);
	}
}
