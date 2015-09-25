#!/usr/bin/python

from dataharvester import *

dh=Tuple("people")
dh.add ( "name", MultiType ( "waldi" ) )
dh["name"]="waldi"
dh["age"]=32.
dh.fill()
dh["name"]="rudi"
dh["age"]=54.
dh.fill()

Writer_file("/tmp/test.txt").save(dh)
Writer_close ()
