#!/usr/bin/python


""" Get the primary vertices out of a CMSSW file!
"""

import dataharvester, sys

file="100vtces.root"

dh=dataharvester.Reader_file ( file )["Events"]

ctr=0

for event in dh:
  vertices=event["recoVertexs_offlinePrimaryVerticesFromCTFTracks__combinedsv"]["obj"]

  for vertex in vertices:
    print ctr,vertex
    ctr=ctr+1
    if ctr>1:
      sys.exit(0)
