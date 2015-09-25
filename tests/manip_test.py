#!/usr/bin/python


""" Test the Filter and the Manipulator.
"""

import dataharvester, sys

file="maniptest.txt.gz"

reader=dataharvester.Reader_file ( file )["Events"]

manipulator=dataharvester.UniversalManipulator()
manipulator.renameColumn("chi2_","chi2")
manipulator.renameColumn("ndof_","ndof")
manipulator.renameColumn("covariance_","covariance")
manipulator.renameColumn("position_.fCoordinates.fX","x")
manipulator.renameColumn("position_.fCoordinates.fY","y")
manipulator.renameColumn("position_.fCoordinates.fZ","z")

manipulator.renameTuple("recoVertexs_offlinePrimaryVerticesFromCTFTracks__combinedsv",\
    "PrimaryVertices")
manipulator.dropTuple("recoVertexs_offlinePrimaryVerticesFromCTFTracks__Rec")

manipulator.dropColumn ("EventAuxiliary" )
manipulator.dropColumn ("weights_.map_")
manipulator.dropColumn ("weights_.ref_.key.product_")
manipulator.dropColumn ("tracks_.refVector_.product_")
manipulator.dropColumn ("tracks_.refVector_.items_")
manipulator.dropColumn ("refittedTracks_")

reader.setManipulator ( manipulator )


event=reader.next()

print event[0]
# vertices=event["PrimaryVertices"]["obj"]
vertices=event["PrimaryVertices"]["obj"]

for vertex in vertices:
  print vertex
