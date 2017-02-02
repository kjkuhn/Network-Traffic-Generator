# Network-Traffic-Generator

this is a simpe and easy to use traffic generator for networks, including a VLAN-tagged ethernet header

## BUILD

In order to build the program goto build and execute 
	cmake .. && make
after this you should able to run the code

## EXECUTION

for command listings start the program without any options

then restart it with sudo and the desired options

tcpdump might be useful to you:
e.g.
	sudo tcpdump -nettti <interface> '(ether dst host <destination address>)'
