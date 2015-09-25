#ifndef ConcreteWriter_H
#define ConcreteWriter_H

#include <string>
#include <map>
#include "AbstractWriter.h"

namespace harvest {
class ConcreteWriter : public AbstractWriter {
public:
  /**
   *  \class ConcreteWriter
   *  A skeleton for a writer
   */
  ConcreteWriter ( const std::string & filename = "" );
  ~ConcreteWriter();

  ConcreteWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );

private:
  std::string theFileName;
};
}

#endif
