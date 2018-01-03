#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/nl80211.h"
#include "../include/wifi.h"

#define DEFAULT_NAME "wlx000f5412d027"

int main(int argc, char** argv){
	struct wifi_nlstate nlstate;
	struct wifi_interface inf;
	int err;
	char* name;
	if(argc>1){
		name = argv[1];
	}else{
		name=DEFAULT_NAME;
	}
	/*get interface*/
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	err = wifi_get_interface_info(&inf, name, &nlstate);
	/*print*/
	if(err<0){
		printf("error while getting interface information  :  %s\n", wifi_geterror(err));
		return err;
	}else{
		print_if(&inf);
	}
}
