# DDoS-Congestion
<p>DDOS-Congestion is a system that leverages a Pre-existing PCAP extractor (STEELISI MIMIC) and then uses its output to generate traffic flow from multiple clients to multiple server connections on two seperate machines in a isolated (Host-Only) network. The idea behind this project is to create a closed network traffic generator that will serve as a sandbox to test DDoS attacks and DDoS detection solutions on. <p>

## Requirements
<p>This project was developed on multiple VM's on Oracle Virtual Box with Ubuntu 20.04 operating system as the choice OS for the VMs. A user must have at least two Ubuntu 20.04 VMs installed (one will host servers and the other will host clients). The user can also have more VMS installed if, for instance, they want to use the environment for a DDoS-Attack (a third VM that can inject one of the other two with a DDoS-attack). Note: These VM's Should be on host-only network to ensure it is a closed network for testing purposes. <p>

<p> Requirements for this project includes : <p>

*	g++
*	iperf
*   Ifupdown
*   Net-tools
*   VIM (Or text editor of choice)
*	Nload
*	Cmake 
*	git (to download mimic repo)
*	libwandio (in order to run mimic)
*	libtrace (in order to run mimic)

<p>All requirements (except for the text editor of choice) for this project are taken care of in the two shell files requirements.sh and mimic-install-build.sh. The user will simply have to run these scripts to install all dependencies for the project. <p>

<P> Run in DDoS-Congestion Directory:

		sh requirements.sh
		sh mimic-install-build.sh
<p>


## Build
1. Run the above commands in the Requirements section to ensure they have all the project's dependencies installed.
2. Compile the CPP files into executables:

		g++ -o parseCSV parseCSV.cpp
		g++ -o ipBind parseCSV.cpp
		g++ -o spawnServers spawnServers.cpp
		g++ -o connectClients connectClients.cpp

 <p> Note: the user can just compile spawnServers.cpp on the machine that will hold the servers(The same can be said for connectClients.cpp). <p>

## Programs
*	ParseCSV.cpp: reads a CSV file (containing ip connect and
event logs) and parses the IP addresses into source and destination ip
files.

<p>Usage:

	./parseCSV <CSV file name>
<p> 

*	ipBind.cpp: reads a file containig IP addresses and binds
them to the hosts machine using a default routing IP (specified by the user). 

<p> Usage:

	./ipBind <.ips File name> <default routing IP>
<p>

*	spawnServers.cpp: reads a user specified file containing IP
addresses and then generates n (the number of IP's in the file) servers
in the background bounded to these IP addresses using the IPERF system
calls.

<p> Usage:

	./spawnServers -s <Destination ips File Name>
<p>

*	connectClients.cpp: reads two user specified files containing
source and destination IP addresses and then makes iperf connections
to running servers (spawned using spawnServers.cpp on another machine)
for a user specified amount of time.

<p> Usage:

	./connectClients -c <Destination ips File Name> <Source ips File Name> <time in seconds for simulation> [OPTIONAL]<bandwidth>
<p>



## Running

### Setting up your VMs Network (between Client and Server machines)

<p> A user must ensure that the Client Machine (we'll call VM A) and the Server Machine (we'll call VM B) are on a closed network and have  unique defualt IP addresses. <p>

1. To ensure the VM's can only talk to eachother click on the VM in Oracle VM Virtualbox Manager and then click settings and then network. Then click the "attached to:" drop-down arow and select "host-only Adapter"(Make sure that all VMs in the network are on host-only adapter or they won't be able to communicate with one another). 

1. If this option doesn't show up you will have to create a host network. To do this back out to the start of Oracle VM Virtualbox Manager and click on "File" in the top left corner. Then click "Host Network Manager". From there you can create your own host Network. Once creating the host network the "host-only Adapter" option from part 1 should appear. If you already have a host only network set up you can skip this step.


1. 	Set up static IP address by going to /etc/network:

		cd /etc/network

 and configure a default IP address by changing the interfaces file to the following:

		# Include files from /etc/network/interfaces.d:
    	#source-directory /etc/network/interfaces.d
    	auto lo 
		iface lo inet loopback
		auto enp0s3
		allow-hotplug enp0s3
		iface enp0s3 inet static
		address 192.168.56.100
		netmask 255.255.255.0


 <p>Where enp0s3 is the default device and 192.168.56.100 will be
 the static IP address of the default device (Note: If the user wants have a different IP address or they have a different device they wish to use they can just replace the device where enp0s3 appears and the ip address where 192.168.56.100 appears.). This process should be done on both server and client VMs and the two must have different static IP addresses.<p>
 <p> After reboot the changes should be made. One thing to take note of this will completely isolate your VMs outside of the closed network. Once this process is completed the user is ready to run the project.
 <p>
 <p>NOTE: If a user wishes to get their VM back online all they have to do is switch the VM's network back NAT (found where host-only adapter was found in the closed network section) and comment out the contents of the interfaces file. <p>

1. Make sure ip addresses are defualt gateways for each VM. To do this the user just needs to run the following command:
	
		sudo route add default gw <current machines static IP address>

### Runthrough Example

<p> Once both machines are set up by following the instructions above, our program can be ran.To start our program we will need a CSV file that contains connection and events between IP addresses. To gain this CSV test file we use mimic's extract feature on a PCAP file. For more information on mimics functionality please read up on its documentation which can be found in the mimic directory (which is created when the user runs mimic-install-build.sh). For this example we will use sample.CSV as an example CSV File. This is just a place holder name for an actual CSV file. A correctly inputted CSV format should have the following format as this example:

	CONN,2,76.10.22.63,49674,->,52.230.222.68,443,10.495166
	EVENT,2,0,76.10.22.63,SEND,101,0,14.495166

A CSV file outputted by mimic's extract feature will have this format. We will also be using the names of the executables listed in the build section. With our environment set up, CSV FILE in hand, and our executables ready we can run through the program. <p>

1.	Parse CSV FIle:
	
		./parseCSV sample.CSV

	<p>Note this will create two ips files (source.ips and destination.ips). This will have to be ran on both machines or the ip files must be shared with both machines. <p>

1.	Bind IPs on both Machines:
	<p>On VM A (client VM) whose static ip address is 192.168.56.100 bind the source ips to the machine:

		./ipBind Source.ips 192.168.56.100
	<p>

	<p>On VM B (server VM) whose static ip address is 192.168.56.180
		bind the destination ips to the machine: 

		./ipBind Destination.ips 192.168.56.180
	<p>

1.	Start up servers
	<p> On VM B run spawnServers to create n (number of ips in destination.ips) servers running in the background:

		./spawnServers -s Destination.ips
	<p>

1.	Connect clients to servers
	<p> On VM A run connectClients to generate traffic between the two VMs for a user specified amount of time (in seconds). For this example the clients will talk to the servers for 60 seconds and the default bandwidth between the connections will be used:

		./connectClients -c Destination.ips Source.ips 60
	<p>

Note: After the user is done running tests the servers will still be up and running on VM B. The user should kill all iperf processes once they are done:

	sudo killall -9 iperf
<p> 