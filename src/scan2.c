#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <netlink/genl/genl.h>

#include "../include/util.h"
#include "../include/network.h"
#include "../include/mem.h"
#include "../include/linuxlist.h"
#include "../include/interface.h"
#include "../include/wifi.h"
#include "../include/scan2.h"


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
	struct scan_args* sp = (struct scan_args*)args;
	const u_char* pt = packet;
	u_int16_t radiotap_hdr_len;
	struct list_head* list_mn = sp->list_mesh_network;
	struct wifi_mesh_network* mn = new_mesh_network();
	struct wifi_mesh_network* actual_mn;
	struct list_head* list_nw = sp->list_network;
	struct wifi_network* nw = new_network();
	struct wifi_network* actual_nw;
	int got_mn = 0, got_nw=0;
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
		if(tag_id == TAG_SSID){
			network_set_ssid(nw,(const char*)pt+2,tag_len);
			got_nw = 1;
		}
		if(tag_id == TAG_DS_PARAMETER_SET && tag_len==1){
			mesh_network_set_channel(mn,pt[2]);
			network_set_channel(nw,pt[2]);
		}
		if(tag_id == TAG_MESH_ID){
			got_mn = 1;
			mesh_network_set_ssid(mn,(const char*)pt+2,tag_len);
		}
		if(tag_id == TAG_MESH_CONFIG){
			mesh_network_set_configuration(mn,pt+2);
		}
		pt += (tag_len+2);
		len -= (tag_len+2);
	}
	
	/*add network to the list if new*/
	if(nw->ssid == NULL){
		got_nw = 0;
	}
	if(got_nw){
		list_for_each_entry(actual_nw, list_nw, entry){
			if(strcmp(actual_nw->ssid,nw->ssid)==0 && actual_nw->channel==nw->channel){
				got_nw = 0;
			}
		}
	}
	if(got_nw){
		list_add(&(nw->entry), list_nw);
	}else{
		del_network(nw);
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
	sleep(3);
	pcap_breakloop(handle);
	return NULL;
}

int scan_network(struct list_head* list_nw, struct list_head* list_mn, char* dev, char* errbuff){
	pcap_t* handle;
	pthread_t thread;
	int err;
	struct scan_args sc;
	
	/*init scan args*/
	sc.list_network = list_nw;
	sc.list_mesh_network = list_mn;
	
	/*open interface and check type of data sniffed*/
	handle = pcap_open_live(dev, BUFSIZE, 1, 1000, errbuff);
	if(handle == NULL){
		return -199;
	}
	if(pcap_datalink(handle)!=DLT_IEEE802_11_RADIO){
		strncpy(errbuff, "Bad type of interface", PCAP_ERRBUF_SIZE);
		return -199;
	}
	
	/*sniff data*/
	err = pthread_create(&thread, NULL, waiting_thread, handle);
	if(err){
		if(err>0){
			err = -err;
		}
		return -err;
	}
	err = pcap_loop(handle, -1, got_packet, (u_char*)(&sc));
	
	/*free memory*/
	if(err==-1){
		err = -199;
		strncpy(errbuff, pcap_geterr(handle), PCAP_ERRBUF_SIZE);
	}else{
		err = 0;
	}
	pcap_close(handle);
	return err;
}

int scan_all_frequencies(struct list_head* list_mn, int* tab_chanels, int size_tab, char* dev, struct wifi_nlstate* nlstate, char* errbuff){
	struct wifi_interface* inf = new_if();
	LIST_HEAD(list_wiphy);
	struct wifi_wiphy* actual_wp;
	struct wifi_wiphy* wiphy = NULL;
	struct list_int* freq_l;
	int freq, chan, i;
	int err = 0;
	LIST_HEAD(list_nw);
	struct wifi_network* actual_nw;
	
	/*get interface information*/
	err = wifi_get_interface_info(inf, dev, nlstate);
	if(err<0){/*can't get information about interface*/
		del_if(inf);
		return err;
	}
	
	/*get wiphy associatedc with interface*/
	err = wifi_get_wiphy(&list_wiphy, nlstate);
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
		err = -ENODEV;
		goto out;
	}
	
	/*get all frequences supported and scan for each in 2GHz band*/
	list_for_each_entry(freq_l, &(wiphy->frequencies->entry), entry){
		freq = freq_l->i;
		if(freq<2500){
			err = wifi_change_frequency(dev, freq, nlstate);
			if(err<0){
				del_mesh_network_list(list_mn);
				goto out;
			}
			err = scan_network(&list_nw, list_mn, dev, errbuff);
			if(err<0){
				del_mesh_network_list(list_mn);
				goto out;
			}
		}
	}
	
	/*count number of networks by channels*/
	for(i=0;i<size_tab;i++){
		tab_chanels[i]=0;
	}
	list_for_each_entry(actual_nw, &list_nw, entry){
		chan = actual_nw->channel;
		if(chan>0 && chan<=size_tab){
			tab_chanels[chan-1]++;
		}
	}
	
	/*free memory*/
	out:
	del_if(inf);
	del_wiphy_list(&list_wiphy);
	if(err!=0 && err!=-199){
		strncpy(errbuff, wifi_geterror(err), PCAP_ERRBUF_SIZE);
	}
	return err;
}
