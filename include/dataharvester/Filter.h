#ifndef dataharvester_Filter_H
#define dataharvester_Filter_H

#include <dataharvester/Tuple.h>
#include <string>

namespace dataharvester {
class Filter {
public:
  /** \class Filter
   *  A Filter is an object which, given a Tuple,
   *  either accepts or rejects the Tuple.
   */

  /// \return true means accept.
  virtual bool accepts ( const Tuple & t ) const = 0;

  /// The name of this filter?
  virtual std::string name() const = 0;

  virtual Filter * clone() const = 0;
};
}

#endif
