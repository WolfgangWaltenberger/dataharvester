#!/usr/bin/python

""" Test the CommentsExpressor.  """

import dataharvester, sys

file="/tmp/me.txt.gz"
# file="color_out"

writer=dataharvester.Writer_file ( file )
# expressor=dataharvester.CommentsExpresser()
collector=dataharvester.CommentsCollector()

t=dataharvester.Tuple ( "Events", "That's an event tuple" )
t["id"]=23
t.describe("Unique ids","id")
t["Vertex:x"]=3.
t.describe("Reconstructed vertex","Vertex")
t.describe("x coordinate of reconstructed vertex","Vertex:x")
t.fill("Vertex")
t.fill()

print t[0]

# print "Now we manipulate!"
# expressor.manipulate ( t )

writer.save ( t )
dataharvester.Writer_close()

print
print "Now we read!"

reader=dataharvester.Reader_file ( file )

for tuple in reader:
  # collector.manipulate ( tuple )
  print tuple
  for row in tuple:
    print row
#    print "row(Vertex)="
#    print row["Vertex"]
#    print row["Vertex"][0]
#    print "row(id)=",row["id"]
