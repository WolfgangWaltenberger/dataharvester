#ifndef GzipWriter_H
#define GzipWriter_H

#include <string>
#include <map>
#include <dataharvester/AbstractWriter.h>
#include <dataharvester/StreamableWriter.h>
#include <iostream>
#include <sstream>
#include <zlib.h>

namespace dataharvester {
class GzipWriter : public AbstractWriter {
public:
  /**
   *  \class GzipWriter
   *  The ".txt" backend for the Writer.
   *  For a description on how to use this thing,
   *  see the Writer class.
   *
   *  Format:
   *  A line in the txt file reads
   *  "ntuplename: name=value; name=value; name=value1,value2,value3"
   *  the characters ";" and "," (and only them)
   *  are automatically "escaped" with a backslash.
   */
  GzipWriter ( const std::string & filename = "" );
  ~GzipWriter();

  void save ( const Tuple &, const std::string & prefix="" );
  GzipWriter * clone ( const std::string & filename ) const;

private:
  void instantiateStreamer ();
  void getTargetFormat();
  void writeStream();

private:
  StreamableWriter * theStreamer;
  std::ostringstream theStream;
  std::string theFileName;
  gzFile theFile;
  std::string theTargetFormat;
};
}

#endif
