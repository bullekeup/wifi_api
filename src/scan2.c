#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <netlink/genl/genl.h>

#include "../include/network.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/interface.h"
#include "../include/wifi.h"
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
	if(err==-2){
		err = 0;
	}
	return err;
}

int scan_all_frequencies(struct list_head* list_mn, char* dev, struct nl_sock* sock, int nl_id){
	struct wifi_interface* inf = new_if();
	LIST_HEAD(list_wiphy);
	struct wifi_wiphy* actual_wp;
	struct wifi_wiphy* wiphy = NULL;
	struct list_int* freq_l;
	int freq;
	
	int err = 0;
	
	/*get interface information*/
	err = wifi_get_interface_info(inf, sock, nl_id, dev);
	if(err<0){/*can't get information about interface*/
		del_if(inf);
		return err;
	}
	
	/*get wiphy associatedc with interface*/
	err = wifi_get_wiphy(&list_wiphy, sock, nl_id);
	if(err<0){/*can't get wiphy list*/
		del_if(inf);
		return err;
	}
	list_for_each_entry(actual_wp, &list_wiphy, entry){
		if(actual_wp->num == inf->wi_phy){
			wiphy = actual_wp;
		}
	}
	if(wiphy == NULL){/*can't get wiphy associated with interface*/
		err = -1;
		goto out;
	}
	
	/*get all frequences supported and scan for each in 2GHz band*/
	list_for_each_entry(freq_l, &(wiphy->frequencies->entry), entry){
		freq = freq_l->i;
		if(freq<2500){
			err = wifi_change_frequency(dev, freq, sock, nl_id);
			if(err<0){
				del_mesh_network_list(list_mn);
				goto out;
			}
			err = scan_network(list_mn, dev);
			if(err<0){
				del_mesh_network_list(list_mn);
				goto out;
			}
		}
	}
	
	/*free memory*/
	out:
	del_if(inf);
	del_wiphy_list(&list_wiphy);
	return err;
}
