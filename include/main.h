#ifndef MAIN_H
#define MAIN_H

struct options{
	int state;
	unsigned short vlan_id;
	unsigned short vlan_prio;
	char *eth;
	unsigned int packet_size;
	unsigned int delay;
	unsigned char eth_dest[6];
	unsigned int ip_dest;
};

extern struct options opts;

#endif /*MAIN_H*/
