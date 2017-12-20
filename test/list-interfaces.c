#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/nl80211.h"

int main(){
	struct nl_sock* socket;
	int nl_id;
	LIST_HEAD(phy_list);
	LIST_HEAD(vir_list);
	LIST_HEAD(mesh_list);
	struct wifi_interface* inf;
	struct wifi_wiphy* wp;
	/*Create lists*/
	socket = create_nl_socket(&nl_id);
	if(socket == NULL){
		return -1;
	}
	wifi_get_wiphy(&phy_list,socket, nl_id);
	wifi_get_interfaces(&vir_list,socket, nl_id);
	wifi_get_mesh_allowed_if(&mesh_list,socket, nl_id);
	/*print*/
	printf("wiphy:\n");
	list_for_each_entry(wp, &phy_list, entry){
		print_wi_phy(wp);
		printf("\n\n");
	}
	printf("--------------------------------\n\nall interfaces : \n");
	list_for_each_entry(inf, &vir_list, entry){
		print_if(inf);
	}
	printf("\n\n--------------------------------\n\nmesh interfaces : \n");
	list_for_each_entry(inf, &mesh_list, entry){
		print_if(inf);
	}
}
