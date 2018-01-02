#ifndef SCAN2_H
#define SCAN2_H

#include <pcap.h>


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void* waiting_thread(void* handle);
int scan_network(struct list_head* list_mn, char* dev);
int scan_all_frequencies(struct list_head* list_mn, char* dev, struct nl_sock* sock, int nl_id);

#endif /*SCAN2_H*/
