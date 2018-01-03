#ifndef SCAN2_H
#define SCAN2_H

#include <pcap.h>

#include "wifi.h"


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void* waiting_thread(void* handle);


/*scan packet during 5 seconds with the monitor interface given by dev to find mesh network
 * the result is a list of mesh network (struct wifi_mesh_network) wich will be referenced by list_mn
 * errbuff should reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE
 * return is negative if an error append, or 0 otherwise
 * if return is -199, errbuff is set to a description of error*/
int scan_network(struct list_head* list_mn, char* dev, char* errbuff);
/*scan all frequencies of 2GHz band during 5 seconds each with the monitor interface given by dev to find mesh network
 * the result is a list of mesh network (struct wifi_mesh_network) wich will be referenced by list_mn
 * errbuff should reference an array with a size supperior or equal to PCAP_ERRBUF_SIZE
 * return is negative if an error append, or 0 otherwise
 * if return is -199, errbuff is set to a description of error*/
int scan_all_frequencies(struct list_head* list_mn, char* dev, struct wifi_nlstate* nlstate, char* errbuff);

#endif /*SCAN2_H*/
