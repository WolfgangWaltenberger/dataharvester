#ifndef dataharvester_NoFilter_H
#define dataharvester_NoFilter_H

#include <dataharvester/Filter.h>

namespace dataharvester {
class NoFilter : public Filter {
public:
  /** \class NoFilter
   *  A filter that lets everything pass.
   */

  bool accepts ( const Tuple & t ) const;

  std::string name () const;

  NoFilter * clone() const;
};
}

#endif
