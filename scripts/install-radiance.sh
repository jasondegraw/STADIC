#!/bin/sh
sudo apt-get install -y git
sudo apt-get install -y csh
sudo apt-get install -y xorg-dev
#sudo apt-get install -y g++
#wget http://www.radiance-online.org/download-install/radiance-source-code/latest-release/latest-release/rad5R0all.tar.gz
#tar xzf rad5R0all.tar.gz
mkdir ray
cd ray
git clone https://github.com/NREL/Radiance.git --branch combined --single-branch .
cd ..
wget http://www.radiance-online.org/download-install/radiance-source-code/archived-releases/latest-release/rad5R0supp.tar.gz
tar xzf rad5R0supp.tar.gz
wget http://download.osgeo.org/libtiff/tiff-4.0.6.tar.gz
mv tiff-4.0.6 ray/src/px/tiff
cd ray
sed --in-place '/more License.txt/d' makeall
sudo ./makeall install < ../scripts/input.txt
