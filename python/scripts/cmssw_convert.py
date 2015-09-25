#!/usr/bin/python


""" Convert in to out.
"""

import cmssw, sys

def usage():
  print "cmssw_convert.py <in> <out>"
  sys.exit(0)

if len(sys.argv)!=3:
  usage()

input=sys.argv[1]
output=sys.argv[2]

reader=cmssw.Reader_file ( input )["Events"]
writer=cmssw.Writer_file ( output )

manipulator=cmssw.CmsManipulator()
reader.setManipulator ( manipulator )

for event in reader:
  writer.save ( event )

cmssw.Write_close()
