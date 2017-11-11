#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/list.h"
#include "../include/interface.h"
#include "../include/wifi.h"

int main(){
	struct nl_sock* socket;
	int nl_id;
	struct wiphy* phy_list;//list
	struct interface* vir_list;//list
	struct interface* mesh_list;
	struct interface* inf;
	struct wiphy* wp;
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
	list_for_each_entry(wp, &phy_list->entry, entry){
		print_wi_phy(wp);
		printf("\n\n");
	}
	printf("--------------------------------\n\nall interfaces : \n");
	list_for_each_entry(inf, &vir_list->entry, entry){
		print_if(inf);
	}
	printf("\n\n--------------------------------\n\nmesh interfaces : \n");
	list_for_each_entry(inf, &mesh_list->entry, entry){
		print_if(inf);
	}
}
