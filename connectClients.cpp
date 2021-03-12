/* 
Description: This CPP file reads two user specified files containing
source and destination IP addresses and then makes iperf connections
to running servers (spawned using spawnServers.cpp on another machine)
for a user specified amount of time.

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
#include <time.h>

using namespace std;

class Thread {
	/*
	Description: class to simulate a thread. 
	*/
	public:
		int status; // 0 for active 1 for ready to connect
    	string sourceIP; // Source Ip Address
		string destIP; // Destination IP Address
		time_t timeStamp; // time process begins
		int testDuration; // will be used to calculate when process is done
		                  
};

void usage(){
	/*
	Description: Prints message to stdout to let the user how to use
		serverSpawns
	*/
	cout << "Usage:" << endl;
	cout << "Generate client connection: " << endl;
	cout << "\tUsage: ./programName -c <Destination IPs File>";
	cout << " <Source IPs File> <time in seconds for simulation> [OPTIONAL]<bandwidth>" << endl;
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
		// line = line.substr(0, line.length()-1);
		ipArr[ipCount] = line;
		ipCount++;
	}

	ipFile.close();
	return 1;
}

int simulateClientFlow(string Dest, string Source, string simDuration, string bandwidth) {
	/*
	Description: For a user defined amount of time this program makes
	client connections to a server (assumption that servers are running
	a different machine) using system calls of IPERF. Parmaters for the
	Iperf system calls are IP addresses from Dest and Source files
	and bandwidth which are provide by the user as parameters.
	Parameters:
		string: Name of file containing destination ip addresses that 
				will serve as clients for calls to iperf
		string: Name of file containing source ip addresses that
				will serve as binding ips for calls to iperf 
		string: amount (in seconds) that the simulation will run ipef
				commands
		string: bandwidth that iperf client side will use for the simulation
	Return:
		int: function status
			 1: success
			-1: failure (couldn't read destination or source file)

		*/
	int numDest = countIPS(Dest);
	int numSource = countIPS(Source);
	if (numDest == -1 or numSource == -1) {
		// problem reading the file
		return -1;
	}

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
	
	//initialize threads table
	Thread threads[numSource];

	for (int i = 0; i < numSource; i++) {
		//cout << "i: " << i << endl;
		Thread t;
		t.status = 0;
		t.sourceIP = "";
		t.destIP = "";
		time(&(t.timeStamp));
		t.testDuration = 0;
		threads[i] = t;	
	}
	


	int testDur = stoi(simDuration); // testDuration set by user

	time_t start, currentT;
	time(&start);
	time(&currentT);
	// ofstream output("output.txt");
	// run for the user specified amount of time (in seconds)
	while(difftime(currentT, start) < testDur) {
		time(&currentT);
		// iterate through every thread in the thread table
		char * cmd;
		for (int i = 0; i < numSource; i++) {
			if (threads[i].status == 0) {
				// thread is currently not active
				// time to assign it values
				
				// randomizing values
				int sIDX = rand() % numSource; //source Index
				int dIDX = rand() % numDest; // destination Index
				int cITR = rand() % 119 + 1; // Intervals can range between 1 and 119 seconds 

				// update thread and start process
				threads[i].status = 1; // thread will now be active
				threads[i].sourceIP = sourceIPS[sIDX];
				threads[i].destIP = destIPS[dIDX];
				threads[i].testDuration = cITR;

				//connect Client
				string testDurStr = to_string(cITR);
				string command = "iperf -c " + threads[i].destIP + " -d -b " + bandwidth +
				                 " -B " + threads[i].sourceIP + " -t " +
				                 testDurStr + " > /dev/null 2>&1 &";
				cout << "Destination IP: " + threads[i].destIP + ", Source IP: " + threads[i].sourceIP << endl;
			
				cmd = &command[0];
				system(cmd);
				time(&(threads[i].timeStamp)); // want to mark timestamp after process starts
			}
			else {
				// thread is currently active (check to see if its done)
				time_t cur;
				time(&cur);
				if (difftime(cur, threads[i].timeStamp) > threads[i].testDuration){
					// thread is done 
					threads[i].status = 0; // make it available
				}
				else {
					// thread is not done. Let it continue working
					continue;
				}
			}
		}
	}
	// output.close();
	return 1;
}

int main(int argc, char **argv) {
	/*
	Description: Makes sure the user gives the correct amount of
	arguments to simulate client flow and then proceeds to simulate
	Clients:
		int: argument count
		char**: arguments the user will give to run the program
				(see usage for what argv should have) 
	Return:
		int: status of program
			 1: success
			-1: user didn't give the correct amount of arguments 
	*/	
	if (argc > 2 and strcmp(argv[1], "-c") == 0) {
		cout << "-c flag found" << endl;
		if (argc > 6) {
			cout << "invalid amount of arguments" << endl;
			usage();
			return -1;
		}
		string bandwidth = "3"; // default bandwidth.
		if (argc > 5){
			bandwidth = argv[5];
		}
		cout << "beginning client connection simulation" << endl;
		simulateClientFlow(argv[2],argv[3], argv[4], bandwidth);
		return 1;
	}
	
	usage();
	return -1;
}
