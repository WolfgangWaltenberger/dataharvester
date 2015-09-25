#ifndef dataharvester_Manipulator_H
#define dataharvester_Manipulator_H

#include <dataharvester/Tuple.h>
#include <string>

namespace dataharvester {
class Manipulator {
public:
  /** \class Manipulator
   *  A Manipulator is an object which
   *  receives a Tuple and manipulates it.
   */

  virtual void manipulate ( Tuple & ) const = 0;

  /**
   *  The name of this manipulator?
   */
  virtual std::string name() const = 0;

  virtual Manipulator * clone() const = 0;
};
}

#endif
