#ifndef CompositeWriter_H
#define CompositeWriter_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/AbstractWriter.h>

namespace dataharvester {
class CompositeWriter : public AbstractWriter {
public:
  /**
   *  \class CompositeWriter
   *  A data sink multiplexer.
   */
  CompositeWriter ( const std::vector < AbstractWriter * > & );

  void add ( AbstractWriter * );

  CompositeWriter & operator<< ( const Tuple & );
  
  void save ( const Tuple & d, const std::string & prefix="" );
  
  virtual CompositeWriter * clone ( const std::string & filename ) const;

  /**
   *  Destructor is also responsible for closing the file
   */
  virtual ~CompositeWriter();
private:
  std::vector < AbstractWriter * > theSinks;
};
}

#endif
