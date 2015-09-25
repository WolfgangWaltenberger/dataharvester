#!/bin/sh

source test_base

echo "Task: Test the Comments system."
echo "Prerequisites: txt, python"

rm -f /tmp/comments.txt

./comments_test.py > /tmp/comments.txt

diff -q /tmp/comments.txt comments.released.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm /tmp/comments.txt; exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff /tmp/comments.txt comments.released.txt; echo "Difference ${fg_reset}"; exit -1; };
