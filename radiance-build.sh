#!/bin/sh
pwd
sed --in-place '/more License.txt/d' makeall
echo '\ny\n' | sudo /bin/sh ./installib
#echo '\n\ny\n2\n\nn\n' | sudo ./makeall install
