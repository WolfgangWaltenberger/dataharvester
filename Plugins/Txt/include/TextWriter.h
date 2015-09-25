#ifndef TextWriter_H
#define TextWriter_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/StreamableWriter.h>
#include <iostream>

namespace dataharvester {
class TextWriter : public StreamableWriter {
public:
  /**
   *  \class TextWriter
   *  The ".txt" Writer backend.
   *
   *  Example:
   *  Event: id=1;
   *  Event:Vertex: x=0.13; y=0.2; z=1.3; cov=0.,0.,0..
   *  Event:fill
   *
   */
  TextWriter ( const std::string & filename = "" );
  TextWriter ( std::ostream & );
  void setStream ( std::ostream &, bool needs_deletion );
  ~TextWriter();

  TextWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );

  std::string quoteStrings ( const MultiType & in ) const;

private:
  void save ( const TupleRow & row, const std::string & pre );
  std::string theFileName;
  std::ostream * theStream;
  bool theStreamNeedsDeletion;
  bool theUseColors;
  int thePrecision;
};
}

#endif
