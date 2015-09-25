#ifndef dataharvester_TupleNameFilter_H
#define dataharvester_TupleNameFilter_H

#include <dataharvester/Filter.h>
#include <string>

namespace dataharvester {
class TupleNameFilter : public Filter {
public:
  /** \class TupleNameFilter
   *  A very simple filter that filters according to
   *  the tuple name
   */

  TupleNameFilter ( const std::string & name );

  bool accepts ( const Tuple & t ) const;
  std::string name() const;

  TupleNameFilter * clone() const;

private:
  std::string theTupleName;
  bool dontCheck;
};
}

#endif
