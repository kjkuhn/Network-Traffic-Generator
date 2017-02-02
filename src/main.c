#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdint.h"
#include "unistd.h"
#include "errno.h"

#include "main.h"
#include "sniffer.h"
#include "sender.h"

#include "netinet/in.h"
#include "sys/socket.h"
#include "arpa/inet.h"

#define DEFAULT_DELAY	500



static const char *options[] = {"vlan-id", "vlan-prio", "sniffer", "sender", "eth", "ps", "delay", "mac-dest", "ip-dest"};

struct options opts;


int process_options(int argc, char **argv)
{
	int i;
	unsigned int temp;
	for(i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			if(strcmp(&argv[i][1], options[0]) == 0)
			{
				i++;
				sscanf(argv[i], "%hu", &opts.vlan_id);
			}
			else if(strcmp(&argv[i][1], options[1]) == 0)
			{
				i++;
				sscanf(argv[i], "%hu", &opts.vlan_prio);
			}
			else if(strcmp(&argv[i][1], options[2]) == 0)
			{
				opts.state = 0;
			}
			else if(strcmp(&argv[i][1], options[3]) == 0)
			{
				opts.state = 1;
			}
			else if(strcmp(&argv[i][1], options[4]) == 0)
			{
				i++;
				opts.eth = argv[i];
			}
			else if(strcmp(&argv[i][1], options[5]) == 0)
			{
				i++;
				sscanf(argv[i], "%u", &temp);
				if(temp <= 1500 - 28)
					opts.packet_size = temp;
				else
					opts.packet_size = 50;
			}
			else if(strcmp(&argv[i][1], options[6]) == 0)
			{
				i++;
				sscanf(argv[i], "%u", &opts.delay);
			}
			else if(strcmp(&argv[i][1], options[7]) == 0)
			{
				i++;
				sscanf(argv[i], "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
					&opts.eth_dest[0], &opts.eth_dest[1], &opts.eth_dest[2],
					&opts.eth_dest[3], &opts.eth_dest[4], &opts.eth_dest[5]);
			}
			else if(strcmp(&argv[i][1], options[8]) == 0)
			{
				i++;
				opts.ip_dest = inet_addr(argv[i]);
			}
		}
	}
}


void print_help()
{
	printf("invalid parameters, possible options:\n\t-vlan-id <id>\n\t-vlan-prio <0..7>\n\t-<sniffer/sender>\n");
	printf("\t-eth <interface>\n\t-ps <packet size>\n\t-delay <delay in us>\n\t-eth-dest <destination mac>\n");
	printf("\t-ip-dest <destination ip>\n\n");
}


int main(int argc, char **argv)
{
	//int i;
	//for(i = 0; i < argc; i++)
	//	printf("%s\n",argv[i]);
	memset(&opts, 0, sizeof(opts));
	opts.state = -1;
	//opts.delay = DEFAULT_DELAY;
	opts.packet_size = 50;
	
	process_options(argc, argv);
	switch(opts.state)
	{
	case 0:
		//sniffer
		sniffer();
		break;
	case 1:
		//sender
		sender();
		break;
	default: 
		print_help();
		break;
	}
	return 0;
}
