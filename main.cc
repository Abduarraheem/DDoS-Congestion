#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <getopt.h>
using namespace std;


const char *const shortopts = ":h:"; // all possible short options

// all long options must correspond one to one with the short options.
const struct option longopts[]{
    /* 
        format {name, has argument, flag, value}: 
        {<name>,
        <has argument> 0 = no argument. 1 = required argument. 2 = has an argument but is optional, 
        <flag> if set to null returns value, else it will return 0 and flag points to a variable and sets val.,
        <value> is the value returned when calling getopt_long.
    }
    */

    {"help", 0, NULL, 'h'},
    {NULL, 0, NULL,  0} // handling invalid long commands.
};

void usage(){
    cout << "Usage: ./<prog name> --help" << endl;
    cout << "Options:" << endl;
    cout << "\t -h or --help \tfor more info." << endl;
    cout << "\t -<insert option here>: \t<insert info of option>" << endl;
}


int main(int argc, char **argv){
    if (argc < 2){
        usage();
        return -1;
    }

    // will contain the option returned from getopt_long.
    // TODO needs a small fix here doing ./a.out --he or something of that sort enters the h case.
    int opt = 0;
    while((opt = getopt_long(argc, argv, shortopts, longopts, NULL)) != -1){
        switch(opt){
            case 'h':
            // insert code here
                usage();
                break;
            // case '': // add your case here.
                // insert code here
                // break;
            default:
                cout << "Usage: ./<prog name> --help" << endl;
                // usage();
                return -1;
        }
    }

    return 0;
}