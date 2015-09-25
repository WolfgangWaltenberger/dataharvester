#ifndef TntWriter_H
#define TntWriter_H

#include <string>
#include <map>
#include <dataharvester/StreamableWriter.h>
#include <iostream>

namespace dataharvester {
class TntWriter : public StreamableWriter {
public:
  /**
   *  \class TntWriter
   *  The hippodraw text-ntuples backend.
   */
  TntWriter ( const std::string & filename = "" );
  TntWriter ( std::ostream & );
  void setStream ( std::ostream &, bool needs_deletion );
  ~TntWriter();

  TntWriter * clone ( const std::string & filename ) const;
  void save ( const std::map < std::string, MultiType > & data, 
              const std::string & name = "Tree" );

  void save ( const Tuple & d, const std::string & prefix="" );

private:
  void save ( const TupleRow & d, const std::string & prefix );
  std::map < std::string, MultiType > theDates;
  std::string theFileName;
  std::ostream * theStream;
  std::string theLastTuple;
  bool theStreamNeedsDeletion;
};
}

#endif
