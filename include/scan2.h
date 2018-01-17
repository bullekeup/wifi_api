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



 /**
  * \fn void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
  * \brief Analyse a packet to find a network managament frame and, in that case, add the network to the list
  * \param args A reference on a struct scan_args
  * \param header Informations about packet sniffed
  * \param packet A reference on the beginning of the packet
  * */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
/* Wait 3 seconds and stop sniffing packet
 *handle should reference a pcap_t */
 /**
  * \fn void* waiting_thread(void* handle)
  * \brief wait 3 seconds and stop the scan
  * \param handle A reference on pcap_t
  * */
void* waiting_thread(void* handle);


/*scan packet during 5 seconds with the monitor interface given by dev to find mesh network
 * the result is a list of normal networks (struct wifi_network) which will be referenced by list_nw and a list of mesh network (struct wifi_mesh_network) which will be referenced by list_mn
 * errbuff should reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE
 * return is negative if an error append, or 0 otherwise
 * if return is -199, errbuff is set to a description of error*/
 /**
  * \fn int scan_network(struct list_head* list_nw, struct list_head* list_mn, char* dev, char* errbuff)
  * \brief scan network on a monitor interface during 3 seconds to find existing network
  * \param list_nw Must reference an empty list which will be filled with the normal networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param list_mn Must reference an empty list which will be filled with the mesh networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param dev The name of the monitor interface on which perform the scan. Can't be NULL
  * \param errbuff Must reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE. 
  * */
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
