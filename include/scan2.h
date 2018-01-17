#ifndef SCAN2_H
#define SCAN2_H

#include <pcap.h>

#include "wifi.h"

//SIZE
#define BUFSIZE 65000
#define IEEE80211_HDR_LEN 24
#define BEACON_MANAGEMENT_FRAME_LEN 12
#define NB_CHANNELS 14

//TAG CODES
#define TAG_SSID 0
#define TAG_MESH_ID 114
#define TAG_MESH_CONFIG 113
#define TAG_DS_PARAMETER_SET 3

struct scan_args{
	struct list_head* list_mesh_network;
	struct list_head* list_network;
};


/* analyse a packet to find inside a mesh network management frame and, if that case, add the mesh network to the list
 * args should reference a list of mesh network (struct wifi_mesh_network)*/
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
/* Wait 3 seconds and stop sniffing packet
 *handle should reference a pcap_t */
void* waiting_thread(void* handle);


/*scan packet during 5 seconds with the monitor interface given by dev to find mesh network
 * the result is a list of normal networks (struct wifi_network) which will be referenced by list_nw and a list of mesh network (struct wifi_mesh_network) which will be referenced by list_mn
 * errbuff should reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE
 * return is negative if an error append, or 0 otherwise
 * if return is -199, errbuff is set to a description of error*/
int scan_network(struct list_head* list_nw, struct list_head* list_mn, char* dev, char* errbuff);
/*scan all frequencies of 2GHz band during 3 seconds each with the monitor interface given by dev to find mesh network and count the number of network by channel
 * the result is a list of mesh network (struct wifi_mesh_network) wich will be referenced by list_mn
 * tab_channel is should reference an Array which idealy have a size of NB_CHANNELS. All frame at index i will be filled with the number of networks in channel i+1
 * size_tab must be the size of the array tab_channels
 * errbuff should reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE
 * return is negative if an error append, or 0 otherwise
 * if return is -199, errbuff is set to a description of error*/
int scan_all_frequencies(struct list_head* list_nw, struct list_head* list_mn, int* tab_chanels, int size_tab, char* dev, struct wifi_nlstate* nlstate, char* errbuff);
#endif /*SCAN2_H*/
