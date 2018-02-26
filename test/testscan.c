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
	struct wifi_nlstate nlstate;
	char* dev;
	char errbuff [PCAP_ERRBUF_SIZE];
	int num_dev;
	int err, i;
	int tab_channel[14];
	
	/*get interface name*/
	if(argc<2){
		dev = DEFAULT_MONITOR_INTERFACE;
	}else{
		dev = argv[1];
	}
	
	/*Get param for nl message*/
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	/*Creeate interface if doesn't exist*/
	num_dev = if_nametoindex(dev);
	if(num_dev==0){//interface doesn't exist
		/*find a wiphy wich can be monitor*/
		err = wifi_get_wiphy_supporting_type(&l_wp,NL80211_IFTYPE_MONITOR,&nlstate);
		if(err<0){
			printf("error while getting wiphy list supporting monitor  :  %s\n", wifi_geterror(err));
			return err;
		}
		wp = list_first_entry_or_null(&l_wp, struct wifi_wiphy, entry);
		if(wp==NULL){
			printf("error : no interface supporting monitor\n");
			return 0;
		}
		/*creating interface*/
		err = wifi_create_interface(dev, NL80211_IFTYPE_MONITOR, wp->num, &nlstate);
		if(err<0){
			printf("error while creating monitor interface  :  %s\n", wifi_geterror(err));
		}
		/*free memory*/
		del_wiphy_list(&l_wp);
	}
	
	/*Down all interface on same wiphy*/
	err = wifi_get_interfaces(&l_if, &nlstate);
	if(err<0){
		printf("error while getting interface list  :  %s\n", wifi_geterror(err));
		return err;
	}
	err = wifi_get_interface_info(&dev_inf, dev, &nlstate);
	if(err<0){
		printf("error while getting interface %s info  :  %s\n", dev, wifi_geterror(err));
		return err;
	}
	list_for_each_entry(inf, &l_if, entry){
		if(inf->wi_phy == dev_inf.wi_phy){
			err = wifi_down_interface(inf->name);
			if(err<0){
				printf("error while down interface %s  :  %s\n",inf->name, wifi_geterror(err));
				return err;
			}
		}
	}
	
	/*Up monitor interface*/
	err = wifi_up_interface(dev);
	if(err<0){
		printf("error while up interface %s  :  %s\n",dev, wifi_geterror(err));
		return err;
	}
	
	/*Perform scan and print result*/
	err = wifi_scan_all_frequencies(NULL, &l_mn, tab_channel, 14, dev, &nlstate, errbuff);
	if(err<0){
		printf("error while scanning  :  %s\n", wifi_geterror(err));
		if(err==-199){
			printf("\t%s\n", errbuff);
			return 0;
		}
	}
	list_for_each_entry(mn, &l_mn, entry){
		print_mesh_network(mn);
	}
	printf("\n------------------------------\n\n");
	for(i=0;i<14;i++){
		printf("Channel %i: %i ntework(s)\n",i+1,tab_channel[i]);
	}
}
