#!/bin/sh

source test_base

echo "Task: Test the TupleNameFilter and the UniversalManipulator."
echo "Prerequisites: gzip, python"

rm -f /tmp/manip.txt

./manip_test.py > /tmp/manip.txt

diff -q /tmp/manip.txt manip.released.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm /tmp/manip.txt; exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff /tmp/manip.txt manip.released.txt; echo "Difference ${fg_reset}"; exit -1; };
