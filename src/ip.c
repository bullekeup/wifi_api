#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <net/if.h>

#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#include "../include/libnetlink.h"
#include "../include/ip.h"


int wifi_start_avahi(char* dev, char* ip){
	char cmd [256];
	char prov [256];
	char* b_addr = "169.254.";
	char* pt;
	int i, nb, last;
	
	/*Check parameters*/
	if(dev==NULL){
		return -EFAULT;
	}
	if(if_nametoindex(dev)==0){
		return -ENODEV;
	}
	
	/*create command*/
	sprintf(cmd, "avahi-autoipd -D %s", dev);
	
	/*test ip address if present and add it to the command if correcte*/
	if(ip!=NULL){
		pt = ip;
		for(i=0;i<8;i++){
			if(ip[i]!=b_addr[i]){
				return -EINVAL;
			}
		}
		nb=0;i=0;last=-1;
		pt = ip+8;
		while(pt!='\0'){
			if(i==2){
				return -EINVAL;
			}
			if(*pt=='.'){
				if(nb>255 || (last == nb && (nb==0||nb==255))){
					return -EINVAL;
				}
				last = nb;
				i++;
				nb=0;
			}else if(*pt>'9' || *pt<'0'){
				return -EINVAL;
			}else{
				nb = nb*10;
				nb = nb+(*pt-'0');
			}
			pt++;
		}
		strcpy(prov, cmd);
		sprintf(cmd, "%s --start=%s",prov, ip);
	}
	
	/*launch avahi deamon*/
	system(cmd);
	
	return 0;
}

int wifi_stop_avahi(char* dev){
	char cmd [256];
	
	/*Check parameters*/
	if(dev==NULL){
		return -EFAULT;
	}
	if(if_nametoindex(dev)==0){
		return -ENODEV;
	}
	
	/*create command*/
	sprintf(cmd, "avahi-autoipd --kill %s", dev);
	
	/*stop avahi deamon*/
	system(cmd);
	return 0;
}

int store_ip(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg){
	struct ip_param* params = arg;
	struct rtattr * tb[IFA_MAX+1];
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	int len = n->nlmsg_len;
	unsigned char* addr;
	int i;
	
	/*Check and parse rtnl message recieved*/
	if(ifa->ifa_index!=params->ifindex || ifa->ifa_family!=AF_INET){
		return 0;
	}
	len -= NLMSG_LENGTH(sizeof(*ifa));
	parse_rtattr(tb, IFA_MAX, IFA_RTA(ifa), len);
	
	/*get address*/
	if(tb[IFA_LOCAL]){
		addr = RTA_DATA(tb[IFA_LOCAL]);
		for(i=0;i<4;i++){
			params->addr[i] = addr[i];
		}
	}
	return 0;
}

int wifi_get_ip_address(unsigned char* addr, char* dev, struct rtnl_handle* rth){
	int err;
	struct ip_param params;
	int i;
	
	/*Check and init parameters*/
	if(addr==NULL || dev==NULL || rth==NULL){
		return -EFAULT;
	}
	params.ifindex = if_nametoindex(dev);
	params.addr = addr;
	if(params.ifindex == 0){
		return -ENODEV;
	}
	for(i=0;i<4;i++){
		addr[i]=0;
	}
	
	/*send rtnl message and wait for answer*/
	err = rtnl_wilddump_request(rth, 0, RTM_GETADDR);
	if(err <0){
		return err;
	}
	err = rtnl_dump_filter(rth, store_ip, &params, NULL, NULL);
	if(err<0){
		return err;
	}
	if(addr[0]==0){
		return -198;
	}
	return 0;
}


int print_ip_address(unsigned char* addr){
	int i;
	if(addr==NULL){
		return -EFAULT;
	}
	for(i=0;i<4;i++){
		printf("%i",addr[i]);
		if(i<3){
			printf(".");
		}
	}
	return 0;
}

