#ifndef dataharvester_CommentsCollector_H
#define dataharvester_CommentsCollector_H

#include <dataharvester/Manipulator.h>
#include <string>
#include <vector>
#include <map>

namespace dataharvester {
class CommentsCollector : public Manipulator {
  /**
   *  CommentsCollector - a manipulator that collects explicit
   *  _description__ - style comments to "real"
   *  ::getDescription() comments.
   *  It is the inverse operation to the CommentsExpressor.
   */
public:
  void manipulate ( Tuple & ) const;
  std::string name() const;
  CommentsCollector * clone() const;
};
}

#endif
