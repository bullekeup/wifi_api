
#include "../include/libnetlink.h"
#include "../include/ip.h"
#include "../include/util.h"

#define DEFAULT_INTERFACE "wlp3s0"


int main(int argc, char** argv){
	unsigned char addr [4];
	char* dev;
	struct rtnl_handle rth;
	int err;
	
	/*get name of interface*/
	if(argc>1){
		dev = argv[1];
	}else{
		dev = DEFAULT_INTERFACE;
	}
	
	/*open rtnl socket*/
	err = rtnl_open(&rth, 0);
	if(err<0){
		printf("error while oppening rtnl socket");
		return 0;
	}
	
	/*get and print ip address*/
	err = wifi_get_ip_address(addr, dev, &rth);
	if(err<0){
		printf("error while getting ip address:%s",wifi_geterror(err));
		return 0;
	}
	err = print_ip_address(addr);
	printf("\n");
	if(err<0){
		printf("error while printing ip address:%s",wifi_geterror(err));
		return 0;
	}
}
