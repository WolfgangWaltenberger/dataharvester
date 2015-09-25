#ifndef WriterBuilder_H
#define WriterBuilder_H

#include <string>
#include <dataharvester/AbstractWriter.h>

namespace dataharvester {
class WriterBuilder {
public:
  /**
   *  \class WriterBuilder
   *  creates and returns the dataharvester, given the filename.
   *  No memory management is done here.
   */

  AbstractWriter * operator() ( std::string name );
};
}

#endif
