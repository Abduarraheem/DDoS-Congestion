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
	public:
		int status; // 0 for active 1 for ready to connect
    	string sourceIP; // Source Ip they're associated to
		string destIP; // Destination IP they're currently assigned to
		time_t timeStamp;
		int testDuration; // how long process is going. Will compare to time stamp
		                  // to see if thread is done. 
};

void usage(){
	cout << "Usage:" << endl;
	cout << "Generate client connection: " << endl;
	cout << "\tUsage: ./programName -c <Destination IPs File>";
	cout << " <Source IPs File> <time in seconds for simulation> [OPTIONAL]<bandwidth>" << endl;
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
		// line = line.substr(0, line.length()-1);
		ipArr[ipCount] = line;
		ipCount++;
	}

	ipFile.close();
	return 1;
}

int simulateClientFlow(string Dest, string Source, string simDuration, string bandwidth) {
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
				string command = "iperf -c " + threads[i].destIP + " -b -d " + bandwidth +
				                 " -B " + threads[i].sourceIP + " -t " +
				                 testDurStr + " > /dev/null 2>&1 &";
				cmd = &command[0];
				// output << cmd << endl;
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