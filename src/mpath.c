#include <stdlib.h>
#include <stdio.h>

#include "../include/mpath.h"

struct wifi_mesh_path* new_mesh_path(){
	struct wifi_mesh_path* res = malloc(sizeof(struct wifi_mesh_path));
	int i;
	if(res==NULL){
		return NULL;
	}
	INIT_LIST_HEAD(&res->entry);
	for(i=0;i<ETH_ALEN;i++){
		res->dest[i] = 0;
		res->next_hop[i] = 0;
	}
	res->metric = -1;
	return res;
}

void print_mesh_path(struct wifi_mesh_path* mp){
	int i;
	printf("dest:");
	for(i=0;i<ETH_ALEN;i++){
		printf("%02x", mp->dest[i]);
		if(i==ETH_ALEN-1){
			printf("\nnext:");
		}else{
			printf(":");
		}
	}
	for(i=0;i<ETH_ALEN;i++){
		printf("%02x", mp->next_hop[i]);
		if(i==ETH_ALEN-1){
			printf("\nmetric:%i\n",mp->metric);
		}else{
			printf(":");
		}
	}
}

void del_mesh_path_list(struct list_head* l){
	struct list_head* pos, *q;
	struct wifi_mesh_path* tmp;
	list_for_each_safe(pos, q, l){
		tmp = list_entry(pos, struct wifi_mesh_path, entry);
		list_del(pos);
		free(tmp);
	}
}
