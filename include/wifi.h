#ifndef WIFI_H
#define WIFI_H

#define SIZE_SOCKET 8192
#define FLAGS 768 //?

/*function called if an error message is recieved
 * arg should be an int* and will be set to the error code given by err->error*/
int error_cb(struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg);
/*function called if a finish message is recieved
 *arg should be an int* and will be set to 0 */
int finish_cb(struct nl_msg *msg, void *arg);
/*function called if a ack message is recieved
 *arg should be an int* and will be set to 0 */
int ack_cb(struct nl_msg *msg, void *arg);
/*function called to create the list of wireless physical device
 *arg should be a struct list* in which the new struct wiphy will be added */
int phy_handler(struct nl_msg *msg, void *arg);
/*function called to create a list of interface
 * arg should be a struct list* in which the new struct interface will be added*/
int if_handler(struct nl_msg *msg, void *arg);
/*Create a new nl_sock and return null 
 * *nl_id will be set to the numeric family identifier corresponding to nl80211*/
struct nl_sock* create_nl_socket(int* nl_id);
/*send teh msg message and wait the answer*/
int send_recv_msg(struct nl_sock* sock, struct nl_msg* msg, struct nl_cb* cb, int nl_id);
/*return the list of wireless physical devices, or NULL if a problem append*/
struct list* get_wi_phy(struct nl_sock* sock, int nl_id);


/*return the list of interfaces (struct interface), or NULL if a problem append*/
struct list* wifi_get_interfaces(struct nl_sock* sock, int nl_id);
/*return the list of interfaces (struct interface) which supported mesh, or NULL if a problem append*/
struct list* wifi_get_mesh_interfaces(struct nl_sock* sock, int nl_id);

#endif /*WIFI_H*/
