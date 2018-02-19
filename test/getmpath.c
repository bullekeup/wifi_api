#include "../include/util.h"
#include "../include/route.h"
#include "../include/mpath.h"

#define DEFAULT_INTERFACE "wlx000f5412d027"

int main(int argc, char** argv){
	LIST_HEAD(lmp);
	char* dev;
	struct wifi_nlstate nlstate;
	struct wifi_mesh_path* mp;
	int err;
	
	if(argc<2){
		dev = DEFAULT_INTERFACE;
	}else{
		dev = argv[1];
	}
	
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while initializing wifi_nlstate  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	err = wifi_get_mesh_path(&lmp, dev, &nlstate);
	if(err<0){
		printf("error while getting mesh path informations  :  %s\n", wifi_geterror(err));
		return err;
	}
	
	list_for_each_entry(mp, &lmp, entry){
		print_mesh_path(mp);
		printf("\n");
	}
}
