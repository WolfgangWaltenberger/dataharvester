#ifndef Reader_H
#define Reader_H

#include <string>
#include <dataharvester/AbstractReader.h>

namespace dataharvester {
class Reader {
public:
  /**
   *  \class Reader
   */

  /**
   *   This is the only way to access the only instantiation.
   */
  static AbstractReader & file ( const std::string & s );
  static void close ( const std::string & s );

private:
  Reader();
  ~Reader();
};
}

#endif
