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
    cout << "Usage: ./<prog name> <file name> <default routing IP>" << endl;
}


int main(int argc, char **argv){
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
// ip route add default via 192.168.56.180 dev enp0s3
// ip addr add 72.20.13.14/24 dev enp0s3
// ip route delete 72.20.13.0/24
// ip route add 72.20.13.0/24 via 192.168.56.180