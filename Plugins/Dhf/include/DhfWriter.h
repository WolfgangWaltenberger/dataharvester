#ifndef DhfWriter_H
#define DhfWriter_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/StreamableWriter.h>
#include <iostream>

namespace dataharvester {
class DhfWriter : public StreamableWriter {
public:
  /**
   *  \class DhfWriter
   *  The ".txt" backend for the Writer.
   *  For a description on how to use this thing,
   *  see the Writer class.
   *
   *  Example:
   *  Event: id=1;
   *  Event:Vertex: x=0.13; y=0.2; z=1.3; cov=0.,0.,0..
   *  Event:fill
   *
   */
  DhfWriter ( const std::string & filename = "" );
  DhfWriter ( std::ostream & );
  void setStream ( std::ostream &, bool needs_deletion );
  ~DhfWriter();

  DhfWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );

private:
  void describeTuple ( const Tuple & d, const std::string & prefix );
  void saveTupleRow ( const TupleRow & row, const std::string & prefix );
  void saveTuple ( const Tuple & d, const std::string & prefix );

private:
  std::string theFileName;
  std::ostream * theStream;
  std::map < std::string, bool > theHasTupleDescription;
  bool theStreamNeedsDeletion;
};
}

#endif
