from dataharvester import UniversalManipulator
import VertexManipulator
import JetTagManipulator
import TrackManipulator

class CmsManipulator(UniversalManipulator):
  def __init__ ( self ):
    UniversalManipulator.__init__( self )
    v=VertexManipulator.VertexManipulator()
    self.add(v)
    j=JetTagManipulator.JetTagManipulator()
    self.add(j)
    t=TrackManipulator.TrackManipulator()
    self.add(t)
