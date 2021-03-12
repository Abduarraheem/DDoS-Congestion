/* 
Description: This CPP file reads a user specified file containing IP
addresses and then generates n (the number of IP's in the file) servers
in the background bounded to these IP addresses using the IPERF system
calls.

Authors: Eugene Tan, Abduarraheem Elfandi, Jackson Klagge

Group Project Name:
   DDoS-DELS (Distributed Denial of Service Detection Evaluatin in Linux Systems)

Course: CIS 433 Computer and Network Security
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <set> 
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

void usage(){
	/*
	Description: Prints message to stdout to let the user how to use
		serverSpawns
	*/
	cout << "Usage:" << endl;
	cout << "Spawn Servers: " << endl;
	cout << "\tUsage: ./programName -s <Destination IPs File>" << endl;
}

int countIPS(string ipFileName){
	/*
	Description: Reads an ipFile of and returns the number of IP addresses
		contained in that file (This file makes the assumption the each IP addr
		is its own line).
	Paramaters: 
		string: the name of the file the program will open and read
	Returns:
		int: 
			ipCount: number of addresses in file or
		         -1: failure (couldn't read file)
	*/

	// initialize line buffer, file object, and ip Counter
	string line;
	fstream ipFile;
	int ipCount = 0;

	// open ip File
	ipFile.open(ipFileName);
	if (ipFile.fail()){
		// if failed to open ipFile return error
		return -1;
	}
	// read through file and return IP count
	while(getline(ipFile, line)){
		ipCount++;
	}

	ipFile.close();
	return ipCount;
}

int fillIPArray(string ipFileName, string *ipArr, int ipArrSize){
	/*
	Description: Reads an ipFile line by line and stores that line
		(an IP address) into a string array.
	Parameters:
		string: name of IPFile that will be opened and read
		string *: string array where IP addresses (read from ipFileName)
					will be stored
		int: size of the string array ipArr
	Returns:
		int: function return status
			1: for success
		   -1: for failure (couldn't read file or file is going past array size)
	*/

	string line;
	fstream ipFile;
	int ipCount = 0;

	// open ip File
	ipFile.open(ipFileName);
	if (ipFile.fail()){
		// if failed to open ipFile return error
		return -1;
	}
	// read through file and return IP count
	while(getline(ipFile, line)){
		if (ipCount == ipArrSize){
			break;
		}
		line = line.substr(0, line.length());
		ipArr[ipCount] = line;
		ipCount++;
	}

	ipFile.close();
	return 1;
}

int generateServers(string Destinations){
	/*
	Description: Creates n (# of IP addresses in Destinations) servers
		in the users background using system calls to iperf.  
	Parameters:
		string: name of IPFile containing destination addresses
	Return:
		int: function status
			1: success
		   -1: Destination file couldn't be read


	*/

	// count up number of Destination ips
	int numDest = countIPS(Destinations);
	if (numDest == -1) {
		// countIPS couldn't read Destination file so
		// we should exit function
		return -1;
	}
	// create list of destination IPS
	string destIPS[numDest];
	fillIPArray(Destinations, destIPS, numDest);


	// start up an iperf2 server in the background for ever dest IP
	// starting at port 5001
	char *cmd;
	string portGen = to_string(numDest);
	string gen = portGen + " ports will be generated";
	cout << gen << endl;
	for (int i = 0; i < numDest; i++) {
		int portNumber = 5001 + i;
		string portStr  = to_string(portNumber);
		string command = "iperf -s -D -p " + portStr + " -B " + destIPS[i];
		cmd = &command[0];
		system(cmd);
	}
	cout << "Server generation completed." << endl;
	return 1;
}


int main(int argc, char **argv) {
	/*
	Description: Makes sure the user gives the correct amount of
	arguments to run spawnServers and then proceeds to spawn Servers
	Parameters:
		int: argument count
		char**: arguments the user will give to run the program
				(see usage for what argv should have) 
	Return:
		int: status of program
			 1: success
			-1: user didn't give the correct amount of arguments 
	*/	

	// make user gives the correct amount of arguments
	if (argc > 2 and strcmp(argv[1], "-s") == 0) {
		cout << "-s flag found" << endl;
		if (argc != 3) {
			cout << "invalid amount of arguments" << endl;
			usage();
			return -1;
		}
		cout << "generating Servers" << endl;
		generateServers(argv[2]);
		return 1;
	}
	usage();
	return -1;

}
