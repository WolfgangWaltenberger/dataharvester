#!/usr/bin/python
"""
.. module:: dataharvester_merge.py
   :synopsis: a script used to merge dh files.
              
.. moduleauthor:: Wolfgang Waltenberger <wolfgang.waltenberger@gmail.com>

"""

import ROOT, os, sys, argparse, types, dataharvester

argparser = argparse.ArgumentParser(description='Merge dataharvested files')
argparser.add_argument ( '-i', '--input', nargs='?', help='the input file', type=types.StringType, default='T3w.root' )
argparser.add_argument ( '-o', '--output', nargs='?', help='the output file', type=types.StringType, default='' )
argparser.add_argument ( '-t', '--tuples', nargs='?', help='the tuples to be merged, empty means merge all tuples. comma-seperated value', type=types.StringType, default='' )
argparser.add_argument ( '-n', '--nmax', nargs='?', help='maximum entries', type=types.IntType, default=-1 )
argparser.add_argument ( '-v', '--verbose', help='verbose', action='store_true' )
argparser.add_argument( '-V','--version', action='version', version=dataharvester.version() )

args=argparser.parse_args()

inputfile=args.input
outputfile=args.output
overwrite=False
if outputfile==inputfile:
  outputfile="tmp.root"
  overwrite=True

if outputfile=="":
  outputfile="merged."+inputfile

f=ROOT.TFile(inputfile)

maxctr=0

keys={}

if args.tuples=="":
  things=f.GetListOfKeys()
  for tree in things:
    keys[tree.GetName()]=True
else:
  for i in args.tuples.split(","):
    keys[i]=True

for name in keys:
  l=ROOT.TList()
  ctr=0
  while True:
    ctr+=1
    tmp=f.Get("%s;%d" % ( name, ctr ) )
    if not tmp:
      break
  ctr-=1
  if ctr>maxctr:
    maxctr=ctr

if maxctr < 2:
  if args.verbose:
    print "[dataharvester-merge] I dont see more than one cycle, leave it all untouched."
  sys.exit(0)

w=ROOT.TFile(outputfile,"recreate")

for name in keys:
  l=ROOT.TList()
  ctr=0
  while True:
    ctr+=1
    tmp=f.Get("%s;%d" % ( name, ctr ) )
    if tmp:
      t1=tmp.CloneTree(args.nmax )
      l.Add(t1)
    else:
      break

  if args.verbose:
    print "[dataharvester-merge] Now merge %s, %d cycles" % ( name, l.GetEntries() )
  pre=f.Get( name )
  ROOT.gROOT.cd() ## produce it in memory
  tn=pre.MergeTrees(l)
  tn.AutoSave("overwrite")
  w.cd() ## now go over to using the file
  tn.Write()

w.Close()
f.Close()

if overwrite:
  if args.verbose:
    print "[dataharvester-merge] Now move %s -> %s" % ( outputfile, inputfile )
  os.system("mv %s %s" % ( inputfile, inputfile.replace(".root",".cycles.root") ) )
  os.system("mv %s %s" % ( outputfile, inputfile ) )

