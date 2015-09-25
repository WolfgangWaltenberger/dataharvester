#!/bin/sh

echo "tcp based code is currently not tested."                                                          

exit 0

source test_base

IP=38823

echo "Task: test the harvestingd."
echo "Prerequisites: harvestingd, HarvestingTest, HarvesterCommand, dataharvester-converter"
echo ""

rm -f f1.txt f1.root
V=0

echo "Produce the master file"
HarvestingTest -d c.root
dataharvester-converter c.root c.txt

echo "start the daemon"
harvestingd -v$V -p $IP &

sleep 1

echo "send the data"

HarvestingTest -v$V -oTcpDataHarvester:SetEndMarker=1 -d dhtp://localhost:$IP/f1.root
sleep 1
HarvesterCommand dhtp://localhost:$IP/f1.root close_all

echo "kill the daemon"
killall -SIGINT harvestingd

echo "(Locally) transfer to txt, to be able to compare"
dataharvester-converter f1.root f1.txt

echo "Compare files"

diff -q f1.txt c.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; exit 0; } || {
	echo "${fg_red}Difference!! "; diff f1.txt c.txt;
};
