#ifndef RootWriter_H
#define RootWriter_H

#include <string>
#include <map>
#include <dataharvester/AbstractWriter.h>
#include "RootDataCache.h"

namespace dataharvester {
class RootWriter : public AbstractWriter {
public:
  /**
   *  \class RootWriter
   *  The root backend for the Writer.
   *  For a description on how to use this thing,
   *  see the Writer class.
   */

  RootWriter ( const std::string & filename = "" );
  RootWriter * clone ( const std::string & filename ) const;
  ~RootWriter();

  void save ( const std::map < std::string, MultiType > &, 
              const std::string & name = "Tree" );
  void save ( const Tuple &, const std::string & prefix="" );

private:
  void save ( const TupleRow & r, const std::string & tuplename, 
              const std::string & prefix );
  void write();

private:
  RootDataCache theCache;
  int theItemsInCache;
  bool theIsClosed;
  std::string theFileName;
  bool theFlushed;
};
}

#endif
