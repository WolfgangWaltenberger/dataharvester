#ifndef GzipReader_H
#define GzipReader_H

#include <string>
#include <map>
#include <fstream>
#include <utility>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/MultiType.h>
#include <zlib.h>

namespace dataharvester {
class GzipReader : public AbstractReader {
public:
  /**
   *  \class GzipReader
   *  A class that is meant to perform the "inverse operation" of
   *  the TextDataHarvester. That is, from a TxtDataHarveste'd
   *  .txt file, it creates the STL std::maps.
   */

  GzipReader ( const std::string & file = "" );
  GzipReader * clone ( const std::string & file ) const;
  ~GzipReader();

  Tuple next ();

  int getNumberOfEntries() const;
  
  std::string filename() const;
  void reset();

private:
  std::string readLine();

private:
  std::string theFileName;
  gzFile theFile;
  std::string theBuffer;
  void openFile();
  static const int theMaxLen = 2048;
};
}

#endif
