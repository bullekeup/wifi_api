#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include <stdio.h>

#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/nl80211.h"

int main(){
	struct wifi_nlstate nlstate;
	LIST_HEAD(phy_list);
	LIST_HEAD(vir_list);
	LIST_HEAD(mesh_list);
	struct wifi_interface* inf;
	struct wifi_wiphy* wp;
	int err;
	/*Create lists*/
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	err = wifi_get_wiphy(&phy_list,&nlstate);
	if(err<0){
		printf("error while getting list of wireless physical device  :  %s\n", wifi_geterror(err));
		return err;
	}
	err = wifi_get_interfaces(&vir_list,&nlstate);
	if(err<0){
		printf("error while getting list of interface  :  %s\n", wifi_geterror(err));
		return err;
	}
	err = wifi_get_if_supporting_type(&mesh_list,NL80211_IFTYPE_MESH_POINT,&nlstate);
	if(err<0){
		printf("error while getting list of interface enabling mesh  :  %s\n", wifi_geterror(err));
		return err;
	}
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
	printf("\n\n--------------------------------\n\ninterfaces supporting mesh: \n");
	list_for_each_entry(inf, &mesh_list, entry){
		print_if(inf);
	}
}
