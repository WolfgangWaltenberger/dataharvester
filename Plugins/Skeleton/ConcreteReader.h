#ifndef ConcreteReader_H
#define ConcreteReader_H

#include <string>
#include <map>
#include "AbstractReader.h"

namespace harvest {
class ConcreteReader : public AbstractReader {
public:
  /**
   *  \class ConcreteReader
   *  A harvest::Reader skeleton
   *  FIXME not yet done
   */

  ConcreteReader ( const std::string & file = "" );
  ConcreteReader * clone ( const std::string & file ) const;

  ~ConcreteReader();

  Tuple next ();
  void reset ();

  int getNumberOfEntries() const;

  std::string filename() const;

private:
  std::string theFileName;
};
}

#endif
