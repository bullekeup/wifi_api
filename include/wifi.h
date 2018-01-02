#ifndef WIFI_H
#define WIFI_H

#include "interface.h"

#define SIZE_SOCKET 8192
#define FLAGS 768 //?

/*function called if an error message is recieved
 * arg should be an int* and will be set to the error code given by err->error */
int error_cb(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
/*function called if a finish message is recieved
 *arg should be an int* and will be set to 0 */
int finish_cb(struct nl_msg *msg, void *arg);
/*function called if a ack message is recieved
 *arg should be an int* and will be set to 0 */
int ack_cb(struct nl_msg *msg, void *arg);
/*function called to create the list of wireless physical device
 *arg should be a struct list_head*  */
int phy_handler(struct nl_msg *msg, void *arg);
/*function called to create a list of interface
 * arg should be a struct list_head*  */
int if_handler(struct nl_msg *msg, void *arg);
/*Create a new nl_sock or return null 
 * *nl_id will be set to the numeric family identifier corresponding to nl80211*/
struct nl_sock* create_nl_socket(int* nl_id);
/*send teh msg message and wait the answer*/
int send_recv_msg(struct nl_sock* sock, struct nl_msg* msg, struct nl_cb* cb, int nl_id);
int send_ifreq(struct ifreq* ifr);



/*make the list of wireless physical devices (struct wifi_wiphy)
 * wiphy will be referenced by list lwp
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_wiphy(struct list_head* lwp, struct nl_sock* sock, int nl_id);
/*make the list of interfaces (struct wifi_interface)
 * interfaces will be referenced by list lif
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_interfaces(struct list_head* lif, struct nl_sock* sock, int nl_id);
/*make the list of interfaces (struct wifi_interface) wich support type indicated by second argument
 * interfaces will be referenced by list if_res
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct nl_sock* sock, int nl_id);
/*make the list of wireless physical device (struct wifi_wiphy) wich support type indicated by second argument
 * wiphy will be referenced by list wp_res
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct nl_sock* sock, int nl_id);
/*get a struct wifi_interface by his name.
 * inf will be completed with informations
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_interface_info(struct wifi_interface* inf, struct nl_sock* sock, int nl_id, char* name);
/*change frequency of the interface designed by name
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_change_frequency(char* name, int freq, struct nl_sock* sock, int nl_id);
int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct nl_sock* sock, int nl_id);
int wifi_up_interface(char* name);
int wifi_down_interface(char* name);
#endif /*WIFI_H*/
