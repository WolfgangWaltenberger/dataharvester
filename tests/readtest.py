#!/usr/bin/python

from tables import *

file = openFile ( 'test.hdf' )

# print "All Nodes:"
# print file

# for node in file:
#   print node

print
print "Groups:"
for group in file(classname="Group"):
  print group

print
print "Ages:"
print file.root.root.People.read(field="Age")

print
print "Married:"
print file.root.root.People.read(field="married")

print
print "Names:"
print file.root.root.People.read(field="Name")
