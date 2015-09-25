#ifndef dataharvester_NoManipulator_H
#define dataharvester_NoManipulator_H

#include <dataharvester/Manipulator.h>
#include <string>
#include <vector>
#include <map>

namespace dataharvester {
class NoManipulator : public Manipulator {
  /**
   *  NoManipulator - a manipulator that does nothing.
   */
public:
  void manipulate ( Tuple & ) const;
  std::string name() const;
  NoManipulator * clone() const;
};
}

#endif
