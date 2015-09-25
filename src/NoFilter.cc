#include <dataharvester/NoFilter.h>

using namespace dataharvester;

bool NoFilter::accepts ( const Tuple & t ) const
{
  return true;
}

NoFilter * NoFilter::clone() const
{
  return new NoFilter(*this);
}

std::string NoFilter::name() const
{
  return "NoFilter";
}
