
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>

#include "../include/wifi.h"
#include "../include/interface.h"
#include "../include/scan2.h"
#include "../include/network.h"
#include "../include/ip.h"
#include "../include/mpath.h"
#include "../include/route.h"
#include "../include/util.h"

#define SIZE_CMD 256

//wiphy show [mesh|monitor]
//interface show [mesh|monitor]
//interface show (interface)
//interface settype (interface) <mesh|monitor>
//interface <up|down> (interface)
//interface new (interface) (wiphy) <monitor|mesh>
//ip (interface)
//avahi <start|stop> (interface)
//mesh join (interface) (meshid)
//mesh create (interface) (meshid) [(channel)]
//scan perform
//network show [mesh|normal]
//network count
//path show (interface)
//path add (interface) (dest) (next_hop)
//path remove (interface) (dest)

struct wifi_nlstate nlstate;
struct rtnl_handle rth;
LIST_HEAD(list_if);//interfaces
struct wifi_interface* inf;
LIST_HEAD(list_wp);//wiphy
struct wifi_wiphy* wiphy;
LIST_HEAD(list_nw);//normal network
struct wifi_network* nw;
LIST_HEAD(list_mn);//mesh network
struct wifi_mesh_network* mn;
LIST_HEAD(list_mp);//mesh path
struct wifi_mesh_path* mp;
int tb_chan [14];
int best_chan, err;

void cmd_wiphy(int argc, char** argv){
	int err;
	if(argc>0){
		if(strncmp(*argv,"show",SIZE_CMD)==0){
			argc--;
			argv++;
			if(argc>0){
				if(strncmp(*argv,"mesh",SIZE_CMD)==0){
					//get and print wiphy supporting mesh
					del_wiphy_list(&list_wp);
					err = wifi_get_wiphy_supporting_type(&list_wp,NL80211_IFTYPE_MESH_POINT,&nlstate);
					if(err<0){
						printf("error: %s\n",wifi_geterror(err));
						return;
					}
					list_for_each_entry(wiphy, &list_wp, entry){
						print_wi_phy(wiphy);
					}
					//
				}else if(strncmp(*argv,"monitor",SIZE_CMD)==0){
					//get and print wiphy supporting monitor
					del_wiphy_list(&list_wp);
					err = wifi_get_wiphy_supporting_type(&list_wp,NL80211_IFTYPE_MONITOR,&nlstate);
					if(err<0){
						printf("error: %s\n",wifi_geterror(err));
						return;
					}
					list_for_each_entry(wiphy, &list_wp, entry){
						print_wi_phy(wiphy);
					}
					//
				}else{
					goto usage;
				}
			}else{
				//get and print all wiphy
				del_wiphy_list(&list_wp);
				err = wifi_get_wiphy(&list_wp,&nlstate);
				if(err<0){
					printf("error: %s\n",wifi_geterror(err));
					return;
				}
				list_for_each_entry(wiphy, &list_wp, entry){
					print_wi_phy(wiphy);
				}
				//
			}
		}else{
			goto usage;
		}
	}else{
		goto usage;
	}
	return;
	usage:
	printf("usage:\n\twiphy show\n\twiphy show mesh\n\twiphy show monitor\n");
}




void cmd_interface(int argc, char** argv){
	int err;
	if(argc==0){
		printf("usage:\n\tinterface show\n\tinterface settype\n\tinterface up\n\tinterface down\n\tinterface new\n");
	}else if(strncmp(*argv,"show",SIZE_CMD)==0){
		argc--;argv++;
		if(argc==0){
			//get and print all interfaces
			del_if_list(&list_if);
			err = wifi_get_interfaces(&list_if,&nlstate);
			if(err<0){
				printf("error: %s\n",wifi_geterror(err));
				return;
			}
			list_for_each_entry(inf, &list_if, entry){
				print_if(inf);
			}
			//
		}else{
			if(strncmp(*argv,"monitor",SIZE_CMD)==0){
				//get and print interface supporting monitor
				del_if_list(&list_if);
				err = wifi_get_if_supporting_type(&list_if, NL80211_IFTYPE_MONITOR, &nlstate);
				if(err<0){
					printf("error: %s\n",wifi_geterror(err));
					return;
				}
				list_for_each_entry(inf, &list_if, entry){
					print_if(inf);
				}
				//
			}else if(strncmp(*argv,"mesh",SIZE_CMD)==0){
				//get and print interface supporting mesh
				del_if_list(&list_if);
				err = wifi_get_if_supporting_type(&list_if, NL80211_IFTYPE_MESH_POINT, &nlstate);
				if(err<0){
					printf("error: %s\n",wifi_geterror(err));
					return;
				}
				list_for_each_entry(inf, &list_if, entry){
					print_if(inf);
				}
				//
			}else{
				//get and print interface info
				inf = new_if();
				err = wifi_get_interface_info(inf, *argv, &nlstate);
				if(err==-ENODEV){
					printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
					return;
				}else if (err<0){
					printf("error: %s\n",wifi_geterror(err));
					return;
				}
				print_if(inf);
				del_if(inf);
				inf = NULL;
				//
			}
		}
	}else if(strncmp(*argv,"settype",SIZE_CMD)==0){
		argc--;argv++;
		if(argc!=2){
			printf("usage:\n\tinterface settype (interface) mesh\n\tinterface settype (interface) monitor\n");
		}else if(strncmp(argv[1],"monitor",SIZE_CMD)==0){
			//set interface in monitor mode
			err = wifi_change_type(*argv, NL80211_IFTYPE_MONITOR, &nlstate);
			if(err==-ENODEV){
				printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
				return;
			}else if (err<0){
				printf("error: %s\n",wifi_geterror(err));
				return;
			}
			//
		}else if(strncmp(argv[1],"mesh",SIZE_CMD)==0){
			//set interface in monitor mode
			err = wifi_change_type(*argv, NL80211_IFTYPE_MESH_POINT, &nlstate);
			if(err==-ENODEV){
				printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
				return;
			}else if (err<0){
				printf("error: %s\n",wifi_geterror(err));
				return;
			}
			//
		}else{
			printf("usage:\n\tinterface settype (interface) mesh\n\tinterface settype (interface) monitor\n");
		}
	}else if(strncmp(*argv,"up",SIZE_CMD)==0){
		argc--;argv++;
		if(argc==0){
			printf("usage: interface up (interface)\n");
		}else{
			//up interface
			err = wifi_up_interface(*argv);
			if(err==-ENODEV){
				printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
				return;
			}else if (err<0){
				printf("error: %s\n",wifi_geterror(err));
				return;
			}
			//
		}
	}else if(strncmp(*argv,"down",SIZE_CMD)==0){
		argc--;argv++;
		if(argc==0){
			printf("usage: interface down (interface)\n");
		}else{
			//down interface
			err = wifi_down_interface(*argv);
			if(err==-ENODEV){
				printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
				return;
			}else if (err<0){
				printf("error: %s\n",wifi_geterror(err));
				return;
			}
			//
		}
	}else if(strncmp(*argv,"new",SIZE_CMD)==0){
		argc--;argv++;
		if(argc!=3){
			printf("usage:\n\tinterface new (interface) (wiphy) mesh\n\tinterface new (interface) (wiphy) monitor\n");
		}else{
			int wp = atoi(argv[1]);
			if(wp<0){
				printf("usage:\n\tinterface new (interface) (wiphy) mesh\n\tinterface new (interface) (wiphy) monitor\n");
				printf("(wiphy) should be a positive or nul integer\n");
			}else if(strncmp(argv[2],"mesh",SIZE_CMD)==0){
				//create a mesh interface
				err = wifi_create_interface(*argv, wp, NL80211_IFTYPE_MESH_POINT, &nlstate);
				if(err<0){
					printf("error: %s\n",wifi_geterror(err));
				}
				//
			}else if(strncmp(argv[2],"monitor",SIZE_CMD)==0){
				//create monitor interface
				err = wifi_create_interface(*argv, NL80211_IFTYPE_MONITOR, wp, &nlstate);
				if(err<0){
					printf("error: %s\n",wifi_geterror(err));
				}
				//
			}else{
				printf("usage:\n\tinterface new (interface) (wiphy) mesh\n\tinterface new (interface) (wiphy) monitor\n");
			}
		}
	}else{
		printf("usage:\n\tinterface show\n\tinterface settype\n\tinterface up\n\tinterface down\n\tinterface new\n");
	}
}



void cmd_ip(int argc, char** argv){
	int err;
	u_int8_t ip [4];
	if(argc==0){
		printf("usage: ip (interface)\n");
	}else{
		//show ip of an interface
		err = wifi_get_ip_address(ip,*argv,&rth);
		if(err==-ENODEV){
			printf("error interface %s does not exist: %s\n",*argv ,wifi_geterror(err));
			return;
		}else if (err<0){
			printf("error: %s\n",wifi_geterror(err));
			return;
		}
		if(ip[0]==0 && ip[1]==0 && ip[2]==0 && ip[3]==0){
			printf("interface %s has no ip address\n",*argv);
		}else{
			printf("ip of %s : ",*argv);
			print_ip_address(ip);
			printf("\n");
		}
		//
	}
}



void cmd_avahi(int argc, char** argv){
	int err;
	if(argc<2){
		printf("usage:\n\tavahi start (interface)\n\tavahi stop interface\n");
	}else if(strncmp(*argv,"start",SIZE_CMD)==0){
		//start avahi
		err = wifi_start_avahi(argv[1], NULL);
		if(err==-ENODEV){
			printf("error interface %s does not exist: %s\n",argv[1] ,wifi_geterror(err));
			return;
		}else if (err<0){
			printf("error: %s\n",wifi_geterror(err));
			return;
		}
		//
	}else if(strncmp(*argv,"stop",SIZE_CMD)==0){
		//stop avahi
		err = wifi_stop_avahi(argv[1]);
		if(err==-ENODEV){
			printf("error interface %s does not exist: %s\n",argv[1] ,wifi_geterror(err));
			return;
		}else if (err<0){
			printf("error: %s\n",wifi_geterror(err));
			return;
		}
		//
	}else{
		printf("usage:\n\tavahi start (interface)\n\tavahi stop interface\n");
	}
}



void mesh_join(int argc, char** argv){
	int err;
	int chan = 0;
	if(best_chan==0){
		printf("please use scan perform to detect existing mesh network\n");
	}else{
		if(argc!=2){
			printf("usage: mesh join (interface) (meshid)\n");
		}else{
			//join a mesh network
			//(find channel of this network)
			list_for_each_entry(mn, &list_mn, entry){
				if(strcmp(mn->meshid, argv[1])==0){
					chan = mn->channel;
				}
			}
			if(chan<=0){
				printf("error: mesh network %s doesn't exist, try mesh create %s %s to create it",argv[1], argv[0], argv[1]);
				return;
			}
			//(check interface is in mesh mode)
			inf = new_if();
			err = wifi_get_interface_info(inf, argv[0], &nlstate);
			if(err==-ENODEV){
				printf("error: interface %s does not exist: %s\n",argv[0] ,wifi_geterror(err));
				return;
			}else if (err<0){
				printf("error while checking %s mode: %s\n",argv[0], wifi_geterror(err));
				return;
			}
			if(inf->type!=NL80211_IFTYPE_MESH_POINT){
				printf("error: interface %s is not in mesh pont mode\n",argv[0]);
				printf("use interface settype %s mesh to solve the problem\n",argv[0]);
				del_if(inf);
				return;
			}
			del_if(inf);
			//(down interface)
			err = wifi_down_interface(argv[0]);
			if(err<0){
				printf("error while downing interface %s: %s\n",argv[0], wifi_geterror(err));
				return;
			}
			//(change frequency)
			err = wifi_change_frequency(argv[0], get_frequency_of_channel(chan),&nlstate);
			if(err<0){
				printf("error while setting %i as frequency of %s: %s\n",get_frequency_of_channel(chan), argv[0], wifi_geterror(err));
			return;
			}
			//(set meshid)
			err = wifi_set_meshid(argv[0], argv[1], &nlstate);
			if(err<0){
				printf("error while setting meshid %s on %s: %s\n",argv[1], argv[0], wifi_geterror(err));
				return;
			}
			//(up interface)
			err = wifi_up_interface(argv[0]);
			if(err<0){
				printf("error while upping interface %s: %s\n",argv[0], wifi_geterror(err));
				return;
			}
			//
		}
	}
}

void mesh_create(int argc, char** argv){
	int err;
	if(argc<2){
		printf("usage: mesh create (interface) (meshid) [(channel)]\n");
	}else{
		int n_chan;
		if(argc==3){
			n_chan = atoi(argv[2]);
			if(n_chan<=0 || n_chan>13){
				printf("channel should be an integer in range [1;13]");
				return;
			}
		}else{
			if(best_chan==0){
				printf("please use scan perform for allowing automatic channel selection\n");
				return;
			}else{
				n_chan = best_chan;
			}
		}
		//create a mesh network
		//(check interface is in mesh mode)
		inf = new_if();
		err = wifi_get_interface_info(inf, argv[0], &nlstate);
		if(err==-ENODEV){
			printf("error: interface %s does not exist: %s\n",argv[0] ,wifi_geterror(err));
			return;
		}else if (err<0){
			printf("error while checking %s mode: %s\n",argv[0], wifi_geterror(err));
			return;
		}
		if(inf->type!=NL80211_IFTYPE_MESH_POINT){
			printf("error: interface %s is not in mesh pont mode\n",argv[0]);
			printf("use interface settype %s mesh to solve the problem\n",argv[0]);
			del_if(inf);
			return;
		}
		del_if(inf);
		//(down interface)
		err = wifi_down_interface(argv[0]);
		if(err<0){
			printf("error while downing interface %s: %s\n",argv[0], wifi_geterror(err));
			return;
		}
		//(change frequency)
		err = wifi_change_frequency(argv[0], get_frequency_of_channel(n_chan),&nlstate);
		if(err<0){
			printf("error while setting %i as frequency of %s: %s\n",get_frequency_of_channel(n_chan), argv[0], wifi_geterror(err));
			return;
		}
		//(set meshid)
		err = wifi_set_meshid(argv[0], argv[1], &nlstate);
		if(err<0){
			printf("error while setting meshid %s on %s: %s\n",argv[1], argv[0], wifi_geterror(err));
			return;
		}
		//(up interface)
		err = wifi_up_interface(argv[0]);
		if(err<0){
			printf("error while upping interface %s: %s\n",argv[0], wifi_geterror(err));
			return;
		}
		//
	}
}

void cmd_mesh(int argc, char** argv){
	if(argc==0){
		printf("usage:\n\tmesh join\n\tmesh create\n");
	}else if(strncmp(*argv,"join",SIZE_CMD)==0){
		mesh_join(argc-1,argv+1);
	}else if(strncmp(*argv,"create",SIZE_CMD)==0){
		mesh_create(argc-1,argv+1);
	}else{
		printf("usage:\n\tmesh join\n\tmesh create\n");
	}
}

void cmd_scan(int argc, char** argv){
	int err;
	char err_buff [PCAP_ERRBUF_SIZE];
	int inf_wp;
	if(argc!=2){
		printf("usage: scan perform (interface)\n");
	}else if(strncmp(*argv,"perform",SIZE_CMD)==0){
		argc--;argv++;
		best_chan = 0;
		//scan for networks
		del_network_list(&list_nw);
		del_mesh_network_list(&list_mn);
		//(check interface is in monitor mode)
		inf = new_if();
		err = wifi_get_interface_info(inf, *argv, &nlstate);
		if(err==-ENODEV){
			printf("error: interface %s does not exist: %s\n", *argv,wifi_geterror(err));
			return;
		}else if (err<0){
			printf("error while checking %s mode: %s\n",*argv, wifi_geterror(err));
			return;
		}
		if(inf->type!=NL80211_IFTYPE_MONITOR){
			printf("error: interface %s is not in monitor mode\n",*argv);
			printf("use interface settype %s monitor to solve the problem\n",*argv);
			del_if(inf);
			return;
		}
		inf_wp = inf->wi_phy;
		del_if(inf);
		//(down all interface on same wiphy)
		//((get all interface))
		err = wifi_get_interfaces(&list_if, &nlstate);
		if(err<0){
			printf("error while getting interface list:  %s\n", wifi_geterror(err));
			return;
		}
		//((down interface if on the same wiphy))
		list_for_each_entry(inf, &list_if, entry){
			if(inf->wi_phy == inf_wp){
				//(((down interface)))
				err = wifi_down_interface(inf->name);
				if(err<0){
					printf("error while down interface %s:  %s\n",inf->name, wifi_geterror(err));
					return;
				}
			}
		}
		//(up interface)
		err = wifi_up_interface(*argv);
		if(err<0){
			printf("error while upping interface %s: %s\n",*argv, wifi_geterror(err));
			return;
		}
		//(perform scan)
		err = wifi_scan_all_frequencies(&list_nw, &list_mn, tb_chan, 14, *argv, &nlstate, err_buff);
		if(err==-199){
			printf("pcap error: %s\n",err_buff);
		}else if(err<0){
			printf("error while downing interface %s:  %s\n",inf->name, wifi_geterror(err));
			return;
		}
		//(get best channel)
		best_chan = get_best_channel(tb_chan, 14);
		//
	}else{
		printf("usage: scan perform (interface)\n");
	}
}

void cmd_network(int argc, char** argv){
	if(best_chan==0){
		printf("use scan perform before using network command to print result\n");
	}else if(argc==0){
		printf("usage:\n\tnetwork show\n\tnetwork show mesh\n\tnetwork show normal\n\tnetwork count\n");
	}else if(strncmp(*argv,"show",SIZE_CMD)==0){
		argc--;argv++;
		if(argc>0){
			if(strncmp(*argv,"mesh",SIZE_CMD)==0){
				//print mesh networks
				list_for_each_entry(mn, &list_mn, entry){
					print_mesh_network(mn);
				}
				//
			}else if(strncmp(*argv,"normal",SIZE_CMD)==0){
				//print normal networks
				list_for_each_entry(nw, &list_nw, entry){
					print_network(nw);
				}
				//
			}else{
				printf("usage:\n\tnetwork show\n\tnetwork show mesh\n\tnetwork show normal\n");
			}
		}else{
			//print all networks
			//(print normal networks)
			printf("-----normal networks-----\n");
			list_for_each_entry(nw, &list_nw, entry){
				print_network(nw);
			}
			//(print mesh networks)
			printf("-----mesh networks-----\n");
			list_for_each_entry(mn, &list_mn, entry){
				print_mesh_network(mn);
			}
			//
		}
	}else if(strncmp(*argv,"count",SIZE_CMD)==0){
		//count networks by channel
		for(int i=0;i<14;i++){
			printf("channel %i : %i networks\n",i+1,tb_chan[i]);
		}
		printf("best channel: %i\n",best_chan);
		//
	}else{
		printf("usage:\n\tnetwork show\n\tnetwork show mesh\n\tnetwork show normal\n\tnetwork count\n");
	}
}

void cmd_path(int argc, char** argv){
	int err;
	if(argc!=2){
		printf("usage: path show (interface)\n");
	}else if(strncmp(*argv,"show",SIZE_CMD)==0){
		argc--;argv++;
		//print paths on mesh interface
		del_mesh_path_list(&list_mn);
		err = wifi_get_mesh_path(&list_mp, *argv, &nlstate);
		if(err<0){
			printf("error: %s\n", wifi_geterror(err));
			return;
		}
		list_for_each_entry(mp, &list_mp, entry){
			print_mesh_path(mp);
		}
		//
		printf("print path of interface %s\n",*argv);//
	}else{
		printf("usage: path show (interface)\n");
	}
}


int parse_cmd(int argc, char** argv){
	if(strncmp(*argv,"wiphy",SIZE_CMD)==0){
		cmd_wiphy(argc-1,argv+1);
	}else if(strncmp(*argv,"interface",SIZE_CMD)==0){
		cmd_interface(argc-1,argv+1);
	}else if(strncmp(*argv,"ip",SIZE_CMD)==0){
		cmd_ip(argc-1,argv+1);
	}else if(strncmp(*argv,"avahi",SIZE_CMD)==0){
		cmd_avahi(argc-1,argv+1);
	}else if(strncmp(*argv,"mesh",SIZE_CMD)==0){
		cmd_mesh(argc-1,argv+1);
	}else if(strncmp(*argv,"scan",SIZE_CMD)==0){
		cmd_scan(argc-1,argv+1);
	}else if(strncmp(*argv,"network",SIZE_CMD)==0){
		cmd_network(argc-1,argv+1);
	}else if(strncmp(*argv,"path",SIZE_CMD)==0){
		cmd_path(argc-1,argv+1);
	}else if(strncmp(*argv,"quit",SIZE_CMD)==0){
		return 0;
	}else{
		printf("Unknown command, possibilities:\n\twiphy\n\tinterface\n\tip\n\tavahi\n\tmesh\n\tscan\n\tnetwork\n\tpath\n\tquit\n");
	}
	return 1;
}

int init(){
	err = wifi_init_nlstate(&nlstate);
	if(err<0){
		printf("error while init nlstate: %s\n",wifi_geterror(err));
		return 0;
	}
	err = rtnl_open(&rth, 0);
	if(err<0){
		printf("error while oppening rtnl socket\n");
		return 0;
	}
	return 1;
}

int main(){
	int ct;
	char cmd [SIZE_CMD];
	int ac, i, pt;
	char* av [SIZE_CMD];
	ct = init();
	while(ct){
		printf(">");
		fgets(cmd, SIZE_CMD, stdin);
		i = 0; ac = 1; pt=1;
		av[0] = cmd;
		while(i<SIZE_CMD && cmd[i]!='\0'){
			if(cmd[i]==' '){
				cmd[i]='\0';
				av[pt] = cmd + i + 1;
				ac++;
				pt++;
			}
			if(cmd[i]=='\n'){
				cmd[i]='\0';
				i--;
			}
			i++;
		}
		ct = parse_cmd(ac,av);
	}
}
