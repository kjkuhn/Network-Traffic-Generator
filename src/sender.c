/* Includes */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "unistd.h"
#include "errno.h"
#include "signal.h"

#include "sys/socket.h"
#include "sys/ioctl.h"

#include "linux/if_packet.h"

#include "net/if.h"

#include "arpa/inet.h"

#include "netinet/ether.h"
#include "netinet/in.h"
#include "netinet/udp.h"
#include "netinet/tcp.h"
#include "netinet/ip.h"

#include "main.h"
#include "sender.h"

/* Macros */
#define DEFAULT_IF		"eth0"
#define DEFAULT_DEST_MAC	"ff:ff:ff:ff:ff:ff"
#define DEFAULT_DEST_IP		"192.168.2.101"
#define DEFAULT_SRC_IP		"111.111.111.111"

/* Structures */
struct vlan_eth{
	uint8_t dst[6];
	uint8_t src[6];
	uint16_t vlan_type;
	uint16_t vlan_data;
	uint16_t ether_type;
}__attribute__((packed));

/* Variables */

static int sd;
static int run = 1;
/* Function Prototypes */

static void error_handler();
static void shutdown_handler(int sig);
static void mton(uint8_t *dest, const char *src);

/* Function Implementations */




void sender()
{
	int len;
	int err;
	uint8_t buffer[1536];
	struct vlan_eth *eth;
	struct iphdr *ip;
	struct udphdr *udp;
	struct sockaddr_ll sa;
	struct ifreq if_idx, if_mac, if_ip;
	char ifName[IFNAMSIZ];

	signal(SIGINT, shutdown_handler);
	if(opts.eth == 0)
		strcpy(ifName, DEFAULT_IF);
	else
		strcpy(ifName, opts.eth);
	if((sd = socket(PF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0)
		error_handler();
	memset(&if_idx, 0, sizeof(if_idx));
	memset(&if_mac, 0, sizeof(if_mac));
	memset(&if_ip, 0, sizeof(if_ip));
	strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
	strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
	if(ioctl(sd, SIOCGIFINDEX, &if_idx) < 0)
		error_handler();
	else
		printf("interface %s has index %d\n", ifName, if_idx.ifr_ifindex);
	if(ioctl(sd, SIOCGIFHWADDR, &if_mac) < 0)
		error_handler();
	else
		printf("HW-ADDR: %hhx:%hhx:%hhx:%hhx:%hhx:%hhx\n",
			((uint8_t*)if_mac.ifr_hwaddr.sa_data)[0], ((uint8_t*)if_mac.ifr_hwaddr.sa_data)[1],
			((uint8_t*)if_mac.ifr_hwaddr.sa_data)[2],((uint8_t*)if_mac.ifr_hwaddr.sa_data)[3],
			((uint8_t*)if_mac.ifr_hwaddr.sa_data)[4],((uint8_t*)if_mac.ifr_hwaddr.sa_data)[5]);
	if(ioctl(sd, SIOCGIFADDR, &if_ip) < 0)
		error_handler();
	else
		printf("IP-ADDR: %s\n", inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
	memset(buffer, 0, 1536);
	eth = (struct vlan_eth*)buffer;
	len = sizeof(struct vlan_eth);
	if(opts.vlan_id <= 0)
		len -= 4;
	ip = (struct iphdr*)&buffer[len];
	len += sizeof(struct iphdr);
	udp = (struct udphdr*)&buffer[len];
	len += sizeof(struct udphdr);
	len += opts.packet_size;

	udp->source = htons(12345);
	udp->dest = udp->source;
	udp->len = htons(sizeof(struct udphdr) + opts.packet_size);

	ip->ihl = 5;
	ip->version = 4;
	ip->id = htons(2332);
	ip->ttl = 42;
	ip->protocol = 17;
	ip->saddr = inet_addr(inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
//	ip->daddr = inet_addr(DEFAULT_DEST_IP);
	ip->daddr = opts.ip_dest;
	ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + opts.packet_size);
	//mton(eth->dst, DEFAULT_DEST_MAC);
	memcpy(eth->dst, opts.eth_dest, 6);
	memcpy(eth->src, if_mac.ifr_hwaddr.sa_data, 6);
	if(opts.vlan_id > 0)
	{
		eth->ether_type = htons(0x0800);
		eth->vlan_type = htons(0x8100);
		eth->vlan_data = htons(((opts.vlan_prio & 0x7) << 13) | (opts.vlan_id & 0x0fff));
	}
	else
	{
		eth->vlan_type = htons(0x0800);
	}
	sa.sll_ifindex = if_idx.ifr_ifindex;
	sa.sll_halen = ETH_ALEN;
	
	while(run)
	{
		if(opts.delay > 0)
		{
			if(usleep(opts.delay) == -1)
			{
				printf("%s\n", strerror(errno));
			}
		}
		if(sendto(sd, buffer, len, 0, (struct sockaddr*)&sa, sizeof(struct sockaddr_ll)) < 0)
		{
			printf("%s\n", strerror(errno));
		}
	}
	close(sd);
	printf("shutdown complete\n");
}


static void error_handler()
{
	printf("%s (CODE %d)\n", strerror(errno), errno);
	close(sd);
	exit(0);
}


static void shutdown_handler(int sig)
{
	printf("\nSIGINT received, shutting down..\n");
	run = 0; 
}


static void mton(uint8_t *dest, const char *src)
{
	sscanf(src, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &dest[0],&dest[1],&dest[2],&dest[3],&dest[4],&dest[5]);
}
