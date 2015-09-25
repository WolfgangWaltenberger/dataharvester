from dataharvester import UniversalManipulator

class VertexManipulator(UniversalManipulator):
  def __init__ ( self ):
    UniversalManipulator.__init__( self )
    self.renameColumn("chi2_","chi2")
    self.renameColumn("ndof_","ndof")
    self.renameColumn("covariance_","covariance")
    self.renameColumn("position_.fCoordinates.fX","x")
    self.renameColumn("position_.fCoordinates.fY","y")
    self.renameColumn("position_.fCoordinates.fZ","z")
    self.renameTuple("recoVertexs_offlinePrimaryVerticesFromCTFTracks__combinedsv",\
                "PrimaryVertices")
    self.dropTuple("recoVertexs_offlinePrimaryVerticesFromCTFTracks__Rec")
    self.dropColumn ("weights_.ref_.key.product_")
    self.dropColumn ("weights_.map_") 
    self.dropColumn ("tracks_.refVector_.product_") 
    self.dropColumn ("tracks_.refVector_.items_") 
    self.dropColumn ("refittedTracks_")
    self.moveTuple ("PrimaryVertices:obj","PrimaryVertices")

