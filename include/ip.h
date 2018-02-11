/**
 * \file ip.h
 * \brief Defines functions relative to ip address
 * \author BUGNOT Thibaut
 * \date 28 january 2018
 * */
#ifndef IP_H
#define IP_H

struct ip_param{
	int ifindex;
	unsigned char* addr;
};

/**
 * \fn int wifi_start_avahi(char* dev, char* ip)
 * \brief Start avahi deamon on an interface
 * \param dev Name of the interface on which start avahi deamon. Can't be NULL
 * \param ip The ip address to priority give to the interface. Must be in 169.254.0.0/16 range. If this adress is already used in network, an other adress will be selected. Can be NULL. In that case, a random adress will be selected in 169.254.0.0/16 range
 * \return A negative integer if a problem happened or 0 otherwise
 * */
int wifi_start_avahi(char* dev, char* ip);
/**
 * \fn int wifi_stop_avahi(char* dev)
 * \brief Stop avahi deamon on an interface
 * \param dev The interface on which avahi deamon is executing. Can't be NULL.
 * \return A negative integer if a problem happened or 0 otherwise
 * */
int wifi_stop_avahi(char* dev);

int store_ip(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg);
int wifi_get_ip_address(unsigned char* addr, char* dev, struct rtnl_handle* rth);
int print_ip_address(unsigned char* addr);


#endif /*IP_H*/
