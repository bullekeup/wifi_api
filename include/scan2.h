#ifndef SCAN2_H
#define SCAN2_H

#include <pcap.h>


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void* waiting_thread(void* handle);
int scan_network(struct list_head* list_mn, char* dev);

#endif /*SCAN2_H*/
