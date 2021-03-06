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
	//cout << "Generate client connection: " << endl;
	//cout << "\tUsage: ./programName -c <Destination IPs File>";
	//cout << " <Source IPs File> <time in minutes for simulation> " << endl;
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

/*
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
		ipArr[ipCount] = line;
		ipCount++;
	}

	ipFile.close();
	return 1;
}
*/
int generateServers(string Destinations){
	// count up number of Destination ips
	int numDest = countIPS(Destinations);
	cout << "numDest" << numDest << endl;

	// start up an iperf2 server in the background for ever dest IP
	// starting at port 5001
	char *cmd;
	string portGen = to_string(numDest);
	string gen = portGen + " ports will be generated";
	cout << gen << endl;
	for (int i = 0; i < numDest; i++) {
		int portNumber = 5001 + i;
		string portStr  = to_string(portNumber);
		string command = "./iperf2 -s -D -p " + portStr;
		cmd = &command[0];
		system(cmd);
	}
	cout << "Server generation completed." << endl;
	return 1;
}
/*
int simulateClientFlow(string Dest, string Source, string simDuration) {
	// count up numbers of destination and source IPs to expect
	int numDest = countIPS(Dest);
	int numSource = countIPS(Source);
	
	// initialize our string arrays of IPs as well as our port array
	string destIPS[numDest];
	string sourceIPS[numSource];
	string ports[numDest];
	
	// fill out the arrays
	fillIPArray(Dest, destIPS, numDest);
	fillIPArray(Source, sourceIPS, numSource);
	for (int i = 0; i < numDest; i++) {
		int portNumber = 5001 + i;
		string portStr = to_string(portNumber);
		ports[i] = portStr;
	}

/*	
	/* Still to Do:
		-Run program for user specified time
		- While running randomize indexes of portnumbers, and 
		    source/ Destination IP adresses as well as a random
		    runtimes and sleep times
		- System commands should be ran in background. */
	
	//return 1;
//}

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
	/*
	if (argc > 2 and strcmp(argv[1], "-c") == 0) {
		cout << "-c flag found" << endl;
		if (argc != 5) {
			cout << "invalid amount of arguments" << endl;
			usage();
			return -1;
		}
		cout << "beginning client connection simulation" << endl;
		simulateClientFlow(argv[2],argv[3], argv[4]);
		return 1;
	}
	*/
	usage();
	return -1;

}