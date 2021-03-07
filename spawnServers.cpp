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
	cout << "Usage:" << endl;
	cout << "Spawn Servers: " << endl;
	cout << "\tUsage: ./programName -s <Destination IPs File>" << endl;
}

int countIPS(string ipFileName){
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
		line = line.substr(0, line.length()-1);
		ipArr[ipCount] = line;
		ipCount++;
	}

	ipFile.close();
	return 1;
}

int generateServers(string Destinations){
	// count up number of Destination ips
	int numDest = countIPS(Destinations);
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
		string command = "./iperf2 -s -D -p " + portStr + " -B " + destIPS[i];
		cmd = &command[0];
		system(cmd);
	}
	cout << "Server generation completed." << endl;
	return 1;
}


int main(int argc, char **argv) {
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