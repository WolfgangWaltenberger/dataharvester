#!/usr/bin/python

""" Plot primary vertex z-coordinate!  """

import cmssw

file="100vtces.txt.gz"

reader=cmssw.Reader_file ( file )["Events"]
reader.setManipulator ( cmssw.VertexManipulator() )

ctr=0
max=10000

from ROOT import TCanvas, TH2F
c1=TCanvas()
h=TH2F("stats","Profile, Primary vertices",20,-100.,100.,20,-100.,100.)

for event in reader:
  ctr=ctr+1
  if ctr>max:
    break

  vertices=event["PrimaryVertices"]

  for vertex in vertices:
    [x,y,z]=[ vertex["x"], vertex["y"], vertex["z"] ]
    if x!=None:
      h.Fill ( 10000.*x, 10000.*y )

h.Draw("box")
h.SetXTitle("x [#mum]")
h.SetYTitle("y [#mum]")
c1.Print("bla.ps")
