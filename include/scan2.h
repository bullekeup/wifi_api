/**
 * \file scan2.h
 * \brief Defines functions and values needed to scan existing networks with pcap library
 * \author BUGNOT Thibaut
 * \date 28 january 2018
 * */
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
  * \param args A reference on a struct scan_args. Function will do nothing if NULL.
  * \param header Informations about packet sniffed
  * \param packet A reference on the beginning of the packet
  * */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
 /**
  * \fn void* waiting_thread(void* handle)
  * \brief wait 3 seconds and stop the scan
  * \param handle A reference on a pcap_t structure. Function will do nothing if NULL.
  * */
void* waiting_thread(void* handle);


 /**
  * \fn int scan_network(struct list_head* list_nw, struct list_head* list_mn, char* dev, char* errbuff)
  * \brief scan network on a monitor interface during 3 seconds to find existing network
  * \param list_nw Must reference an empty list which will be filled with the normal networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param list_mn Must reference an empty list which will be filled with the mesh networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param dev The name of the monitor interface on which perform the scan. Can't be NULL.
  * \param errbuff Must reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE. Can't be NULL. Will be filled with a error message if an error relative to pcap happend.
  * \return a negative integer if an error happened or 0 otherwise. If return is -199, array referenced by previous param will be set with a description of error.
  * */
int scan_network(struct list_head* list_nw, struct list_head* list_mn, char* dev, char* errbuff);
  /**
  * \fn int scan_all_frequencies(struct list_head* list_nw, struct list_head* list_mn, int* tab_chanels, int size_tab, char* dev, struct wifi_nlstate* nlstate, char* errbuff);
  * \brief scan all frequences of 2GHz band on a monitor interface during 3 seconds each to find existing network
  * \param list_nw Must reference an empty list which will be filled with the normal networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param list_mn Must reference an empty list which will be filled with the mesh networks found during the scan. Can be NULL if user doesn't want this type of informations
  * \param tab_channels Must reference an array which will be filled with the numbers of networks using each channel (tab_channel[i] will be set to the number of network using channel i+1). Better if size is at less 14. Can be NULL if user doesn't want this type of informations.
  * \param size_tab the size of the array referenced by previous argument. Numbers of network on channels with number supperior to this parameter won't be counted. Should be 0 if previous argument is NULL.
  * \param dev The name of the monitor interface on which perform the scan. Can't be NULL.
  * \param nlstate A reference on a initialised struct wifi_nlstate. Can't be NULL
  * \param errbuff Must reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE. Can't be NULL. Will be filled with a error message if an error relative to pcap happend.
  * \return a negative integer if an error happened or 0 otherwise. If return is -199, array referenced by previous param will be set with a description of error.
  * */
int scan_all_frequencies(struct list_head* list_nw, struct list_head* list_mn, int* tab_chanels, int size_tab, char* dev, struct wifi_nlstate* nlstate, char* errbuff);
#endif /*SCAN2_H*/
