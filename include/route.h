#ifndef ROUTE_H
#define ROUTE_H

/*#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>*/
#include <netlink/attr.h>
#include "nl80211.h"
#include "util.h"
#include "wifi.h"

int mpath_handler(struct nl_msg *msg, void *arg);

int wifi_add_mesh_path(char* name, char* dest, char* next_hop, struct wifi_nlstate* nlstate);

int wifi_get_mesh_path(struct list_head* lmp, char* name, struct wifi_nlstate* nlstate);

#endif /*ROUTE_H*/
