#ifndef dataharvester_CommentsExpresser_H
#define dataharvester_CommentsExpresser_H

#include <dataharvester/Manipulator.h>
#include <string>
#include <vector>
#include <map>

namespace dataharvester {
class CommentsExpresser : public Manipulator {
  /**
   *  CommentsExpresser - a manipulator that turns Tuple descriptions into "data
   *  members".  that can be stored by Writers that do not support "meta
   *  information" directly.
   *  It "expresses" the comments. It is the inverse operation to
   *  the CommentsCollector.
   */
public:
  void manipulate ( Tuple & ) const;
  std::string name () const;
  CommentsExpresser * clone() const;
};
}

#endif
