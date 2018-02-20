#include <stdlib.h>
#include <netlink/errno.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "../include/util.h"

struct nlparam* new_nlparam(enum nl80211_attrs attr, int type, int value_int, char* value_str){
	struct nlparam* res = malloc(sizeof(struct nlparam));
	res->attr = attr;
	res->type = type;
	res->value_int = value_int;
	res->value_str = value_str;
	INIT_LIST_HEAD(&res->entry);
	return res;
}

void del_nlparam_list(struct list_head* nlpl){
	struct list_head* pos, *q;
	struct nlparam* tmp;
	list_for_each_safe(pos, q, nlpl){
		tmp = list_entry(pos, struct nlparam, entry);
		list_del(pos);
		free(tmp);
	}
}





struct list_int* new_list_int(){
	struct list_int* i = malloc(sizeof(struct list_int));
	INIT_LIST_HEAD(&i->entry);
	return i;
}



const char* wifi_geterror(int err){
	if(err<0){
		err = -err;
	}
	if(err==199){
		return "pcap error";
	}
	if(err<200){
		return strerror(err);
	}else{
		return nl_geterror(err-200);
	}
}

void mac_addr_data_to_str(char* str,const u_int8_t* data){
	int i, pt=0;
	char prov [5];
	for(i=0;i<ETH_ALEN;i++){
		sprintf(prov, "%02x", data[i]);
		str[pt] = prov[0];
		str[pt+1] = prov[1];
		if(i<ETH_ALEN-1){
			str[pt+2] = ':';
		}else{
			str[pt+2]='\0';
		}
		pt += 3;
	}
}

u_int8_t hex_to_dec(char h){
	switch (h){
		case 'a':case 'A':
		return 0xa;
		case 'b':case 'B':
		return 0xb;
		case 'c':case 'C':
		return 0xc;
		case 'd':case 'D':
		return 0xd;
		case 'e':case 'E':
		return 0xe;
		case 'f':case 'F':
		return 0xf;
		default:
		return h-'0';
	}
}

int mac_addr_str_to_data(u_int8_t* data,const char* str){
	int i, pt=0;
	char h1,h2;
	for(i=0;i<ETH_ALEN*3;i+=3){
		h1 = hex_to_dec(data[i]);
		h2 = hex_to_dec(data[i+1]);
		if(h1>0xf || h1<0 || h2>0xf || h2<0 || (pt<ETH_ALEN-1 && data[i+2]!=':')){
			return -EINVAL;
		}
		data[pt] = (h1<<4)+h2;
		pt++;
	}
	return 0;
}
