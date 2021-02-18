#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <set> 

using namespace std;

void usage(){
    cout << "Usage: ./<prog name> <CSV file name>" << endl;
}
int main(int argc, char **argv){
    if (argc < 2){
        usage();
        return -1;
    }

    set <string> source;
    set <string> dest;
    string line;
    fstream file;
    file.open(argv[1]);
    if(file.fail()){
        cout << "File could not be opened." << endl;
        return -1;
    }
    while(getline(file, line)){
        if (line.find("CONN") != string::npos){      // check if it is a connections between two IPs.
            vector<string> connection;
            string token;
            istringstream ls {line};
            while(getline(ls, token, ',')){
                connection.push_back(token);
            }
            string sourceIP = connection[2];        // get source IP
            if(!dest.count(sourceIP)){             // if source IP not in dest set then just add to source.
                source.insert(sourceIP);
            }
            string destIP = connection[5];        // get dest IP.
            if(!source.count(destIP)){           // if dest IP not in source set then just add to dest.
                dest.insert(destIP);
            }

        }
    }
    file.close();

    ofstream srcFile("Source.ips");
    for(set<string>::iterator it = source.begin(); it != source.end(); ++it){
        srcFile << *it << endl; // put source IP to file.
    }
    srcFile.close();
    ofstream destFile("Destination.ips");
    for(set<string>::iterator it = dest.begin(); it != dest.end(); ++it){
        destFile << *it << endl; // put dest IP to file.
    }
    destFile.close();

    return 0;
}