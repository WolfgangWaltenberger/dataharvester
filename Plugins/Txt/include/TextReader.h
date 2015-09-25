#ifndef TextReader_H
#define TextReader_H

#include <string>
#include <map>
#include <fstream>
#include <utility>
#include <vector>
#include <dataharvester/StreamableReader.h>
#include <dataharvester/MultiType.h>

namespace dataharvester {
class TextReader : public StreamableReader {
public:
  /**
   *  \class TextReader
   *  Backend that is capable of reading csv-style .txt files.
   *  (See the TextWriter for what the format looks like).
   */

  TextReader ( std::string file = "" );
  TextReader ( std::istream * );
  // FIXME need a stream interface as well
  TextReader * clone ( const std::string & file ) const;
  TextReader ( const TextReader & o );
  void setStream ( std::istream *, bool needs_deletion );

  ~TextReader();

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
  std::string theFileName;
  std::istream * theStream;
  bool theStreamNeedsDeletion;
  int theLineNr;

  bool getLine ( std::string & ret ) const;
  bool parseLine ( std::string, std::string &, std::map < std::string, MultiType > & );
  bool openFile();
  Tuple theCurrentTuple;
};
}

#endif
