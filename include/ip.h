#ifndef IP_H
#define IP_H

/**
 * \fn int wifi_start_avahi(char* dev, char* ip)
 * \brief Start avahi deamon on an interface
 * \param dev Name of the interface on which start avahi deamon. Can't be NULL
 * \param ip The ip address to priority give to the interface. Must be in 169.254.0.0/16 range. If this adress is already used in network, an other adress will be selected. Can be NULL. In that case, a random adress will be selected in 169.254.0.0/16 range
 * \return A negative integer if a problem append or 0 otherwise
 * */
int wifi_start_avahi(char* dev, char* ip);
/**
 * 
 * */
int wifi_stop_avahi(char* dev);

#endif /*IP_H*/
