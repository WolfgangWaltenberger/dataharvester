#ifndef XmlWriter_H
#define XmlWriter_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/StreamableWriter.h>
#include <iostream>

namespace dataharvester {
class XmlWriter : public StreamableWriter {
public:
  /**
   *  \class XmlWriter
   *  The writing backend for xml.
   */
  XmlWriter ( std::string filename = "" );
  XmlWriter ( std::ostream & );
  void setStream ( std::ostream &, bool needs_deletion );
  ~XmlWriter();

  XmlWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );
  void printHeader() const;
  void printFooter() const;

private:
  void save ( const TupleRow &, const std::string & prefix, const std::string & name );
  void setFileName(); //< implements filename[doctype].xml syntax
  std::map < std::string, MultiType > theDates;
  // have we already written down the tuple description?
  std::map < std::string, bool > theHasDescription;
  // have we already written down the column description?
  std::map < std::string, std::map < std::string, bool > > theHasCDescription;
  std::string theFileName;
  std::string theDocType;
  std::ostream * theStream;
  bool theStreamNeedsDeletion;
};
}

#endif
