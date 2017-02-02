# Network-Traffic-Generator

this is a simpe and easy to use traffic generator for networks, including a VLAN-tagged ethernet header

for command listings start the program without any options

then restart it with sudo and the desired options

tcpdump might be useful to you:
e.g.
	sudo tcpdump -nettti <interface> '(ether dst host <destination address>)'
