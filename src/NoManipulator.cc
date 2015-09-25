#include <dataharvester/NoManipulator.h>

using namespace dataharvester;

NoManipulator * NoManipulator::clone() const
{
  return new NoManipulator ( *this );
}

void NoManipulator::manipulate ( Tuple & t ) const
{
}

std::string NoManipulator::name() const
{
  return "NoManipulator";
}
