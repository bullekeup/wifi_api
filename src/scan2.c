#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "../include/network.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/scan2.h"


#define BUFSIZE 65000
#define IEEE80211_HDR_LEN 24
#define BEACON_MANAGEMENT_FRAME_LEN 12
#define TAG_MESH_ID 114
#define TAG_MESH_CONFIG 113
#define TAG_DS_PARAMETER_SET 3

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
	const u_char* pt = packet;
	u_int16_t radiotap_hdr_len;
	struct list_head* list_mn = (struct list_head*)args;
	struct wifi_mesh_network* mn = new_mesh_network();
	struct wifi_mesh_network* actual_mn;
	int got_mn = 0;
	int len = header->len;
	/*get radiotap_header length and skip it*/
	radiotap_hdr_len = pt[2] + (pt[3]<<8);
	pt += radiotap_hdr_len;
	len -= radiotap_hdr_len;
	/*get packet type and skip ieee 802.11 header*/
	if(pt[0]>>2 != 0x20){
		del_mesh_network(mn);
		return;
	}
	pt += IEEE80211_HDR_LEN;
	len -= IEEE80211_HDR_LEN;
	/*skip fixed parameters*/
	pt += BEACON_MANAGEMENT_FRAME_LEN;
	len -= BEACON_MANAGEMENT_FRAME_LEN;
	/*analyse tags*/
	while(len>0){
		int tag_id = pt[0];
		int tag_len = pt[1];
		if(tag_id == TAG_DS_PARAMETER_SET && tag_len==1){
			mesh_network_set_channel(mn,pt[2]);
		}
		if(tag_id == TAG_MESH_ID){
			got_mn = 1;
			mesh_network_set_name(mn,(const char*)pt+2,tag_len);
		}
		if(tag_id == TAG_MESH_CONFIG){
			mesh_network_set_configuration(mn,pt+2);
		}
		pt += (tag_len+2);
		len -= (tag_len+2);
	}
	/*add mesh network to list if new*/
	if(got_mn){
		list_for_each_entry(actual_mn, list_mn, entry){
			if(strcmp(actual_mn->name, mn->name) == 0){
				got_mn = 0;
			}
		}
	}
	if(got_mn){
		list_add(&(mn->entry), list_mn);
	}else{
		del_mesh_network(mn);
	}
}

void* waiting_thread(void* handle){
	sleep(5);
	pcap_breakloop(handle);
	return NULL;
}

int scan_network(struct list_head* list_mn, char* dev){
	pcap_t* handle;
	pthread_t thread;
	char errbuf [PCAP_ERRBUF_SIZE];
	int err;
	/*open interface and check type of data sniffed*/
	handle = pcap_open_live(dev, BUFSIZE, 1, 1000, errbuf);
	if(handle == NULL){
		printf("Can not open device %s : %s\n",dev,errbuf);
		return -1;
	}
	if(pcap_datalink(handle)!=DLT_IEEE802_11_RADIO){
		printf("Bad type interface");
		return -1;
	}
	/*sniff data*/
	err = pthread_create(&thread, NULL, waiting_thread, handle);
	if(err){
		printf("Error in thread creation");
		return -1;
	}
	err = pcap_loop(handle, -1, got_packet, (u_char*)list_mn);
	return err;
}
