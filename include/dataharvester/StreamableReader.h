#ifndef StreamableReader_H
#define StreamableReader_H

#include <iostream>
#include <dataharvester/AbstractReader.h>

namespace dataharvester {
class StreamableReader : public AbstractReader {
public:
  /**
   *  \class StreamableReader
   *  streamable reader can be given input line by line
   */
  /// add a line, returning true if tuple is finished.
  virtual bool addLine ( const std::string & s ) = 0;

  virtual Tuple getCurrentTuple() = 0; // get current tuple 

  virtual void clear() = 0; // clear current tuple

  // StreamableReader can read from streams, as well.
  virtual void setStream ( std::istream *, bool needs_deletion ) = 0;
};
}

#endif
