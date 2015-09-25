#ifndef ReaderBuilder_H
#define ReaderBuilder_H

#include <string>
#include <dataharvester/AbstractReader.h>

namespace dataharvester {
class ReaderBuilder {
public:
  /**
   *  \class ReaderBuilder
   *  creates and returns the dataharvester, given the filename.
   *  No memory management is done here.
   */

  AbstractReader * operator() ( const std::string & name );
};
}

#endif
