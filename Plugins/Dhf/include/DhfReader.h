#ifndef DhfReader_H
#define DhfReader_H

#include <string>
#include <map>
#include <fstream>
#include <utility>
#include <vector>
#include <dataharvester/StreamableReader.h>
#include <dataharvester/MultiType.h>

namespace dataharvester {
class DhfReader : public AbstractReader {
public:
  /**
   *  \class DhfReader
   *  A class that is meant to perform the "inverse operation" of
   *  the TextDataHarvester. That is, from a TxtDataHarveste'd
   *  .txt file, it creates the STL std::maps.
   */
 
  DhfReader ( std::string file = "" );
  DhfReader ( std::istream * );
  // FIXME need a stream interface as well
  DhfReader * clone ( const std::string & file ) const;
  DhfReader ( const DhfReader & o );
  void setStream ( std::istream *, bool needs_deletion );

  ~DhfReader();

  /**
   *  Currently you can iterate thru by
   *  while ( nextNtuple(...) ) { ... };
   */
  bool nextNtuple ( std::string &, std::map < std::string, MultiType > & );

  Tuple next ();
  void reset ();

  /**
   *  Special method that lets the user feed the input
   *  line by line
   */
  bool nextNtuple ( std::string &, std::map < std::string, MultiType > &, std::string );
  bool addLine ( const std::string & );
  bool addToTuple ( std::string ); // add info of one line to current tuple
  bool addData ( std::string ); // add "data row" to current tuple
  void clear(); // clear current tuple
  Tuple getCurrentTuple(); // get current tuple (needed for gzip)

  int getNumberOfEntries() const;

  std::string filename() const;

private:
  /// Read the file signature (make sure its a dhf)
  bool readSignature() const;
  /// Define a tuple.
  void defineTuple ( std::string );
  void readTupleRow ( std::string, const std::string & prefix = "" );
  // void readSubTuple ( const std::string,
  bool getLine ( std::string & ret ) const;
  bool parseLine ( std::string, std::string &, std::map < std::string, MultiType > & );
  bool openFile();

private:
  std::string theFileName;
  std::istream * theStream;
  bool theStreamNeedsDeletion;
  int theLineNr;
  mutable std::map < std::string, Tuple > theEmptyTuples;

  Tuple theCurrentTuple;
};
}

#endif
