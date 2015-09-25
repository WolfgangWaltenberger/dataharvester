#!/bin/sh

source test_base

echo "Task: test the SystemWriter"
echo "Prerequisites: dataharvester-systemwriter, dataharvester-converter"

rm -f /tmp/test0
dataharvester-systemwriter -f /tmp/test0.txt

test -e /tmp/test0.txt || { echo "${fg_red}No File!${fg_reset}"; exit -1; }

echo ""
echo "See if we have well-formed data:"
dataharvester-converter test0.txt new0.txt
diff -q test0.txt new0.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm -f /tmp/head0.txt; rm -f new0.txt exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff test0.txt new0.txt; echo "Difference ${fg_reset}"; exit -1; };
