#!/usr/bin/python

import dataharvester,sys, time

def read ( file ):
  before=time.clock()
  reader=dataharvester.Reader_file(file)

  for tuple in reader:
    pass
  secs=time.clock()-before
  print file,"	",secs

files = [ "bla.txt", "bla.txt.gz", "bla.hdf", "bla.root", "bla.xml", "bla.dhf", "bla.dhf.gz" ]

if len ( sys.argv ) > 1:
  files = sys.argv
  files.pop(0)

for file in files:
  read ( file )
