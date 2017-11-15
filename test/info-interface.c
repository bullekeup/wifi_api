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
	struct nl_sock* socket;
	struct wifi_interface* inf;
	int nl_id, err;
	char* name;
	if(argc>1){
		name = argv[1];
	}else{
		name=DEFAULT_NAME;
	}
	/*get interface*/
	socket = create_nl_socket(&nl_id);
	if(socket == NULL){
		return -1;
	}
	inf = new_if();
	err = wifi_get_interface_info(inf, socket, nl_id, name);
	/*print*/
	if(err == -3){
		printf("no interface names %s\n", name);
	}else if(err <0){
		printf("an error appened\n");
	}else{
		print_if(inf);
	}
}
