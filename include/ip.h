/**
 * \file ip.h
 * \brief Defines functions relative to ip address
 * \author BUGNOT Thibaut
 * \date 28 january 2018
 * */
#ifndef IP_H
#define IP_H
#include <sys/types.h>

struct ip_param{
	int ifindex;
	u_int8_t* addr;
};

/**
 * \fn int wifi_start_avahi(const char* dev, const char* ip)
 * \brief Start avahi deamon on an interface
 * \param dev Name of the interface on which start avahi deamon. Can't be NULL
 * \param ip The ip address to priority give to the interface. Must be in 169.254.0.0/16 range. If this adress is already used in network, an other adress will be selected. Can be NULL. In that case, a random adress will be selected in 169.254.0.0/16 range
 * \return A negative integer if a problem happened or 0 otherwise
 * */
int wifi_start_avahi(const char* dev, const char* ip);
/**
 * \fn int wifi_stop_avahi(const char* dev)
 * \brief Stop avahi deamon on an interface
 * \param dev The interface on which avahi deamon is executing. Can't be NULL.
 * \return A negative integer if a problem happened or 0 otherwise
 * */
int wifi_stop_avahi(const char* dev);
/**\fn int store_ip(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
 * \brief get ip from an rtnl message and store it.
 * \param arg Must reference a struct ip_param.
 * */
int store_ip(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg);
/**
 * \fn int wifi_get_ip_address(u_int8_t* addr, const char* dev, struct rtnl_handle* rth)
 * \brief get ip address of an interface
 * \param addr Must reference a 4 length array which will be filled with the ip address found. Can't be NULL.
 * \param dev A reference of the name of the interface. Can't be NULL.
 * \param rth A reference on a struct rtnl_handle. struct rtnl_handle must has been oppened with rtnl_open. Can't be NULL
 * \return A negative integer if a problem happened, 0 otherwise.
 * */
int wifi_get_ip_address(u_int8_t* addr, const char* dev, struct rtnl_handle* rth);

/**\fn int print_ip_address(const u_int8_t* addr)
 * \brief print an ip address given by 32 bits
 * \param addr A reference on a 4 length array where binary ip address is stored. Can't be NULL.
 * \return A negative integer if a problem happened, 0 otherwise.
 * */
int print_ip_address(const u_int8_t* addr);


#endif /*IP_H*/
