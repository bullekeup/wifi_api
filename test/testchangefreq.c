#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/nl80211.h"

#define DEFAULT_INTERFACE "mon0"
#define DEFAULT_FREQ 2422

int main(int argc, char** argv){
	char* dev;
	int freq;
	struct wifi_nlstate nlstate;
	int err;
	
	if(argc>=2){
		dev = argv[1];
	}else{
		dev = DEFAULT_INTERFACE;
	}
	if(argc>=3){
		freq =  atoi(argv[2]);
	}else{
		freq = DEFAULT_FREQ;
	}
	
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	err = wifi_change_frequency(dev, freq, &nlstate);
	if(err<0){
		printf("error while changing frequency  :  %s\n", wifi_geterror(err));
		return err;
	}
}
