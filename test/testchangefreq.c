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
	struct nl_sock* socket;
	int nl_id;
	
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
	
	socket = create_nl_socket(&nl_id);
	if(socket == NULL){
		return -1;
	}
	
	wifi_change_frequency(dev, freq, socket, nl_id);
}
