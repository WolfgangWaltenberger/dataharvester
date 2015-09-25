#ifndef StreamableWriter_H
#define StreamableWriter_H

#include <iostream>
#include <dataharvester/AbstractWriter.h>

namespace dataharvester {
class StreamableWriter : public AbstractWriter {
public:
  /**
   *  \class StreamableWriter
   *  Streamable dataharvesters are those that can write to
   *  a stream rather than a file.
   */
  virtual void setStream ( std::ostream &, bool needs_deletion ) = 0;
};
}

#endif
