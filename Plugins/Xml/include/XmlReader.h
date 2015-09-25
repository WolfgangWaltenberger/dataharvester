#ifndef XmlReader_H
#define XmlReader_H

#include <string>
#include <map>
#include <fstream>
#include <utility>
#include <vector>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/MultiType.h>
#include <libxml++/parsers/textreader.h>                                                                        

namespace dataharvester {
class XmlReader : public AbstractReader {
public:
  /**
   *  \class XmlReader
   *  The reading backend for xml.
   */

  XmlReader ( std::string file = "" );
  XmlReader * clone ( const std::string & file ) const;
  XmlReader ( const XmlReader & o );

  ~XmlReader();

  /**
   *  Currently you can iterate thru by
   *  while ( nextNtuple(...) ) { ... };
   */
  // bool nextNtuple ( std::string &, std::map < std::string, MultiType > & );

  Tuple next ();
  void reset ();

  /**
   *  Special method that lets the user feed the input
   *  line by line
   */
  bool nextNtuple ( std::string &, std::map < std::string, MultiType > &, std::string );
  bool addLine ( std::string );
  bool addToTuple ( std::string ); // add info of one line to current tuple
  bool addData ( std::string ); // add "data row" to current tuple

  int getNumberOfEntries() const;

  std::string filename() const;

private:
  void addSubTuple ( Tuple & t, const std::string & prefix ) const;

private:
  xmlpp::TextReader * theReader;
  bool hasRead;
  std::string theFileName;
  bool openFile();
};
}

#endif
