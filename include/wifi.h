#ifndef WIFI_H
#define WIFI_H

#include <net/if.h>

#include "interface.h"
#include "linuxlist.h"
#include "mem.h"
#include "nl80211.h"
#include "util.h"

#define SIZE_SOCKET 8192
#define FLAGS 768 //?

struct wifi_nlstate{
	struct nl_sock* sock;
	int nl_id;
};


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
/*send teh msg message and wait the answer*/
int send_recv_msg(struct wifi_nlstate* nlstate, enum nl80211_commands cmd, int flags, struct list_head* params, nl_recvmsg_msg_cb_t func, void* arg);
/*send an interface request*/
int send_ifreq(struct ifreq* ifr);



/* return a error message for an error code*/
const char* wifi_geterror(int err);
/*fill the struct_nlstate pointed by parameter with value needed to use other functions
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_init_nlstate(struct wifi_nlstate* nlstate);
/*make the list of wireless physical devices (struct wifi_wiphy)
 * wiphy will be referenced by list lwp
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_wiphy(struct list_head* lwp, struct wifi_nlstate* nlstate);
/*make the list of interfaces (struct wifi_interface)
 * interfaces will be referenced by list lif
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_interfaces(struct list_head* lif, struct wifi_nlstate* nlstate);
/*make the list of interfaces (struct wifi_interface) wich support type indicated by second argument
 * interfaces will be referenced by list if_res
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_if_supporting_type(struct list_head* if_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/*make the list of wireless physical device (struct wifi_wiphy) wich support type indicated by second argument
 * wiphy will be referenced by list wp_res
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_wiphy_supporting_type(struct list_head* wp_res, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/*get a struct wifi_interface by his name.
 * inf will be completed with informations
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_get_interface_info(struct wifi_interface* inf, char* name, struct wifi_nlstate* nlstate);
/*change frequency of the interface designed by name
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_change_frequency(char* name, int freq, struct wifi_nlstate* nlstate);
/*change type of the interface designed by name
 * return will be negative if a problem append, or 0 otherwise*/
int wifi_change_type(char* name, enum nl80211_iftype type, struct wifi_nlstate* nlstate);
/*Create an interface, with name and type given by respectively first and second parameter, on wireless physical device designed by third parameter
 *return will be negative if a problem append, or 0 otherwise */
int wifi_create_interface(char* name, enum nl80211_iftype type, int wiphy, struct wifi_nlstate* nlstate);
/*Up the interface designed by name 
 *return will be negative if a problem append, or 0 otherwise */
int wifi_up_interface(char* name);
/*Down the interface designed by name
 *return will be negative if a problem append, or 0 otherwise */
int wifi_down_interface(char* name);
#endif /*WIFI_H*/
