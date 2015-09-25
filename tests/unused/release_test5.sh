#!/bin/sh

echo "tcp based code is currently not tested."

exit 0

source test_base

echo "Task: test the harvestingd."
echo "Prerequisites: harvestingd"
echo ""
rm -f via_daemon.txt

echo "start the daemon"

PORT=40823
harvestingd -v0 -p ${PORT} &

sleep 5

echo "send the data"
dataharvester-converter -oTcpDataHarvester:SetEndMarker=1 -v0 test.txt dhtp://localhost:${PORT}/via_daemon.txt
dataharvester-converter -oTcpDataHarvester:SetEndMarker=1 -v0 arrays.txt dhtp://localhost:${PORT}/arrays_via_daemon.txt

echo "kill the daemon"
killall -SIGINT harvestingd # FIXME that's a bit wild

echo "Compare files"

diff test.txt via_daemon.txt
diff -q via_daemon.txt test.txt && {
	echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm via_daemon.txt; exit 0; } || {
	echo "${fg_red}Difference!! You may wish to check yourself:"; 
	diff via_daemon.txt test.txt; echo "Difference ${fg_reset}"; exit -1; };
