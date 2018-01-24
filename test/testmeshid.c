#include <stdio.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include <net/if.h>

#include "../include/nl80211.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/interface.h"
#include "../include/wifi.h"

#define DEFAULT_INTERFACE "wlx00c140770d47"
#define DEFAULT_MESHID "meshIdExemple"

int main(int argc, char** argv){
	int err;
	struct wifi_nlstate nlstate;
	struct wifi_interface inf;
	char* dev;
	char* meshid;
	
	/*get informations from arguments*/
	if(argc<2){
		dev = DEFAULT_INTERFACE;
	}else{
		dev = argv[1];
	}
	if(argc<3){
		meshid = DEFAULT_MESHID;
	}else{
		meshid = argv[2];
	}
	
	/*Get param for nl message*/
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	/*Down the interface*/
	err = wifi_down_interface(dev);
	if(err<0){
		printf("error while down interface %s  :  %s\n",dev, wifi_geterror(err));
		return err;
	}
	
	/*set interface in mesh mode if didn't*/
	err = wifi_get_interface_info(&inf, dev, &nlstate);
	if(err<0){
		printf("error while getting information about interface %s  :  %s\n",dev, wifi_geterror(err));
		return err;
	}
	if(inf.type != NL80211_IFTYPE_MESH_POINT){
		err = wifi_change_type(dev, NL80211_IFTYPE_MESH_POINT, &nlstate);
		if(err<0){
			printf("error while changing type of interface %s  :  %s\n",dev, wifi_geterror(err));
			return err;
		}
	}
	
	/*set meshid*/
	err = wifi_set_meshid(dev, meshid, &nlstate);
	if(err<0){
		printf("error while setting mesh id  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	/*up interface*/
	err = wifi_up_interface(dev);
	if(err<0){
		printf("error while down interface %s  :  %s\n",dev, wifi_geterror(err));
		return err;
	}
}
