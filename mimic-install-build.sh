#! /bin/bash

# downloading the dependencies of mimic

sudo apt-get update
sudo apt-get install libwandio1
sudo apt-get install libtrace3-dev
sudo apt-get install git
# downloading mimic repo off of github
git clone https://github.com/STEELISI/mimic.git

# building mimic's extract and replay feature
cd mimic
sudo sh cmake-install.sh
sudo apt install cmake
cmake CMakeLists.txt
make
sudo make install

# increasing socket limit of mimic through unlimit.sh
cd replay
sudo sh unlimit.sh

# returning user back to original directory
cd ..
cd ..
