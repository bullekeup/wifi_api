#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/nl80211.h"
#include "../include/wifi.h"

#define NAME "wlx000f5412d027"

int main(){
	struct nl_sock* socket;
	int nl_id;
	struct wifi_interface* inf;
	/*Create lists*/
	socket = create_nl_socket(&nl_id);
	if(socket == NULL){
		return -1;
	}
	inf = wifi_get_interface_info(socket, nl_id, NAME);
	/*print*/
	print_if(inf);
}
