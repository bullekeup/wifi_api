#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/list.h"
#include "../include/interface.h"
//#include "../include/nl80211.h"
#include "../include/wifi.h"

int main(){
	struct nl_sock* socket;
	int nl_id;
	struct list* phy_list;
	struct list* vir_list;
	struct list* mesh_list;
	int max, j;
	/*Create lists*/
	socket = create_nl_socket(&nl_id);
	if(socket == NULL){
		return -1;
	}
	phy_list = get_wi_phy(socket, nl_id);
	vir_list = wifi_get_interfaces(socket, nl_id);
	mesh_list = wifi_get_mesh_interfaces(socket, nl_id);
	/*print*/
	printf("wiphy:\n");
	max = size_list(phy_list);
	for(j=0;j<max;j++){
		print_wi_phy(get_from_list(phy_list, j));
		printf("\n\n");
	}
	printf("--------------------------------\n\nall ainerfaces : \n");
	max = size_list(vir_list);
	for(j=0;j<max;j++){
		print_if(get_from_list(vir_list, j));
	}
	printf("\n\n--------------------------------\n\nmesh interfaces : \n");
	max = size_list(mesh_list);
	for(j=0;j<max;j++){
		print_if(get_from_list(vir_list, j));
	}
}
