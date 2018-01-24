#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../include/ip.h"


int wifi_start_avahi(char* dev, char* ip){
	char cmd [256];
	char prov [256];
	char* b_addr = "169.254.";
	char* pt;
	int i, nb, last;
	
	/*create command*/
	sprintf(cmd, "avahi-autoipd -D %s", dev);
	
	/*test ip address if present and add it to the command if correcte*/
	if(ip!=NULL){
		pt = ip;
		for(i=0;i<8;i++){
			if(ip[i]!=b_addr[i]){
				return -198;
			}
		}
		nb=0;i=0;last=-1;
		pt = ip+8;
		while(pt!='\0'){
			if(i==2){
				return -198;
			}
			if(*pt=='.'){
				if(nb>255 || (last == nb && (nb==0||nb==255))){
					return -198;
				}
				last = nb;
				i++;
				nb=0;
			}else if(*pt>'9' || *pt<'0'){
				return -198;
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
	
	/*create command*/
	sprintf(cmd, "avahi-autoipd --kill %s", dev);
	
	/*launch avahi deamon*/
	system(cmd);
	return 0;
}

