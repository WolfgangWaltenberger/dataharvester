#!/usr/bin/python

""" Access the tracks of a CMSSW file!  """

import dataharvester

# file="one.root"
file="1evt_tracks.root"

dh=dataharvester.Reader_file ( file )["Events"]

# ctr=0

for event in dh:
  tracks=event["recoTracks_ctfWithMaterialTracks__Rec"]["obj"]
  print tracks

  # print event["recoTrackExtras_ctfWithMaterialTracks__Rec"]["obj"][0]

  # print event["recoVertexs_offlinePrimaryVerticesFromCTFTracks__combinedsv"]["obj"][0]

  #for track in tracks:
  #  print track
  #  ctr=ctr+1
  #  if ctr>0:
  #    break

