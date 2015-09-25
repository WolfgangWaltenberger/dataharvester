#!/usr/bin/python

import dataharvester

w=dataharvester.Writer_file ( "bla.sqlite" )
t=dataharvester.Tuple("event")
t["id"]=1
t["track:id"]=1
t.fill("track")
t["track:id"]=2
t.fill("track")
t.fill()
w.save(t)

dataharvester.Writer_close()

