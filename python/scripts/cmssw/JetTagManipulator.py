from dataharvester import UniversalManipulator

class JetTagManipulator(UniversalManipulator):
  def __init__ ( self ):
    UniversalManipulator.__init__( self )
    self.dropColumn("EventAuxiliary")
    self.dropTuple("recoCombinedSVTagInfos_combinedSVBJetTags__combinedsv")
    self.dropTuple("recoCombinedSVTagInfos_combinedSVJetTags__combinedsv")
    self.dropTuple("recoCombinedSVTagInfos_combinedSVBJetTags__Rec")
    self.dropTuple("recoCombinedSVTagInfos_combinedSVJetTags__Rec")
    self.moveTuple ("recoJetTags_combinedSVBJetTags__combinedsv:obj","CombinedSVBTag")
    self.moveTuple ("recoJetTags_combinedSVJetTags__combinedsv:obj","CombinedSVTag")
    self.moveTuple ("edmHepMCProduct_source__Rec:obj","MCProduct")
    self.renameColumn("m_discriminator","d")
    self.dropColumn("m_tagInfo.holder_")
    self.dropColumn ("present")
