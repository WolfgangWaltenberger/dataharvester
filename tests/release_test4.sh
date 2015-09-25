#!/bin/sh

source test_base

echo "Task: test array functionality"
echo "Prerequisites: zlib"
echo ""
rm -f final.txt arrays.txt.gz

echo "convert to txt.gz"
dataharvester-converter -v0 arrays.txt arrays.txt.gz

echo "convert to txt"
dataharvester-converter -v0 arrays.txt.gz final.txt
rm arrays.txt.gz

echo "Compare files"

diff arrays.txt final.txt
diff -q final.txt arrays.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm final.txt; exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff final.txt arrays.txt; echo "Difference ${fg_reset}"; exit -1; };
