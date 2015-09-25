#/bin/sh

./cmssw_vtces.py > vtces.txt

diff -q vtces.txt release.txt && {
	        echo "${fg_green}No difference! -> Test OK${fg_reset}"; rm vtces.txt; exit 0; } || {
				  echo "${fg_red}Difference!! You may wish to check yourself:"; 
					diff vtces.txt release.txt; echo "Difference ${fg_reset}"; exit -1; };

