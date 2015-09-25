#ifndef SimpleConfigurable_H
#define SimpleConfigurable_H

#include <string>
#include <dataharvester/SimpleConfiguration.h>

namespace dataharvester {
template < class T> class SimpleConfigurable
{
  /**
   *  Simple implementation of CARF's SimpleConfigurable.
   *  Is only used in the standalone version.
   */
  public:
    SimpleConfigurable ( T value, std::string name ) : theName(name)
    {
      SimpleConfiguration::current()->setDefault ( name, value );
    };
    T value() { return SimpleConfiguration::current()->value(theName); };
  private:
      std::string theName;
};
}

#endif
