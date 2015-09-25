#!/bin/sh

source test_base

echo "Task: go through an extensive \`\`conversion chain'', starting with test.txt."
echo "      compare final output with the original file."
echo "Prerequisites: dataharvester-converter, root, zlib, hdf, xml"
echo ""
rm -f final.txt test.root test.txt.gz
dataharvester-converter test.txt test.root
dataharvester-converter test.root test.hdf
dataharvester-converter test.hdf test.txt.gz
dataharvester-converter test.txt.gz test.xml
dataharvester-converter test.xml final.txt

echo ""
echo "Now diffing final.txt and test.txt:"
diff -q final.txt test.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm test.root test.txt.gz test.hdf test.xml final.txt; exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff final.txt test.txt; echo "Difference ${fg_reset}"; exit -1; };
