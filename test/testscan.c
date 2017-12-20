#include <stdio.h>

#include "../include/network.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/scan2.h"

#define DEFAULT_MONITOR_INTERFACE "mon0"

int main(int argc, char** argv){
	LIST_HEAD(l_mn);
	struct wifi_mesh_network* mn;
	char* dev;
	if(argc<2){
		dev = DEFAULT_MONITOR_INTERFACE;
	}else{
		dev = argv[1];
	}
	scan_network(&l_mn,dev);
	list_for_each_entry(mn, &l_mn, entry){
		print_mesh_network(mn);
	}
}
