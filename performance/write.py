#!/usr/bin/python

import dataharvester,sys, time

def write ( file, n=1000 ):
  before=time.clock()
  writer=dataharvester.SystemWriter(file)
  for i in range(n):
    writer.save()
  dataharvester.Writer_close(file)
  secs=time.clock()-before
  print file,"	",secs

files = [ "bla.txt", "bla.txt.gz", "bla.hdf", "bla.root", "bla.xml", "bla.dhf", "bla.dhf.gz" ]

if len ( sys.argv ) > 1:
  files = sys.argv
  files.pop(0)

for file in files:
  write ( file, 1000 )
