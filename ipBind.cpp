/* 
Description: This CPP file reads a file containig IP addresses and binds
them to the hosts machine using a default routing IP (specified by the user).

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
using namespace std;
#include <cstring>

void usage(){
    /*
    Description: Prints message to stdout to let the user how to use
        ipBind
    */
    cout << "Usage: ./<prog name> <file name> <default routing IP>" << endl;
}


int main(int argc, char **argv){
    /*
    Description: Reads a file (argv[1]) containg IP addresses and uses
    a series of system calls on thoses IP addresses to bind them with
    a default routing IP specified by the user (argv[2]) 
    Parameters:
        int: argument count
        char**: arguments the user will give to run the program
                (see usage for what argv should have)
    Return:
        int: program status
            -1: failed (failed to open file or user arguments are invalid)
             1: Success  
    */
    if (argc < 3){
        usage();
        return -1;
    }
    string line;
    fstream file;
    file.open(argv[1]);
    if(file.fail()){
        cout << "File could not be opened." << endl;
        return -1;
    }

    string defaultIP = argv[2];
    string command = "ip route add default via " + defaultIP + " dev enp0s3";
    int n = command.length() + 1;
    char *cmd;
    cmd = &command[0];
    system(cmd);
    cout << cmd << endl;

    while(getline(file, line)){
        line = line.substr(0, line.length());
        command = "ip addr add " + line + "/24 dev enp0s3";
        cmd = &command[0];
        system(cmd);
        int index = line.find_last_of('.');

        string ip = line.substr(0, index+1) + "0";
        command = "ip route delete " + ip + "/24";
        cmd = &command[0];
        system(cmd);

        command = "ip route add " + ip + "/24 via " + defaultIP;
        cmd = &command[0];
        system(cmd);

    }
    file.close();
    return 0;
}
