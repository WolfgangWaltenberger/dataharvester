#ifndef AbstractWriter_H
#define AbstractWriter_H

#include <string>
#include <map>
#include <dataharvester/MultiType.h>
#include <dataharvester/Tuple.h>

namespace dataharvester {
class AbstractWriter {
public:
  /**
   *  \class AbstractWriter
   *  Purely abstract baseclass for all concrete Writer implementations.
   *
   *  Save data into an ntuple called "name", with "description" ( of the
   *  ntuple )
   */

  /**
   * Save a Tuple
   */
  virtual void save ( const Tuple &, const std::string & prefix = "" ) = 0;

  /**
   *  A streamer which simply calls ::save.
   */
  AbstractWriter & operator<< ( const Tuple & );

  /**
   *  Old way of saving data, flat structures only.
   *  Obsolete.
   */
  void saveFlat ( const std::map < std::string, MultiType > & data, 
                  const std::string & name = "Tree" );

  virtual AbstractWriter * clone ( const std::string & filename ) const = 0;
  /**
   *  Destructor is also responsible for closing the file
   */
  virtual ~AbstractWriter() {};
};
}

#endif
