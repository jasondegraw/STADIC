#!/bin/sh
wget http://www.radiance-online.org/download-install/radiance-source-code/latest-release/latest-release/rad5R0all.tar.gz
tar xvf rad5R0all.tar.gz
cd ray
sed --in-place '/more License.txt/d' makeall
echo '\ny\n' | sudo ./installib
echo '\n\ny\n2\n\nn\n' | sudo ./makeall install
