#!/usr/bin/python

import dataharvester
r=dataharvester.Reader_file ( "bla.sqlite" )
for tuple in r:
  for row in tuple:
    for data in row:
      print data,"=",row[data]
