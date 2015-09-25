#include <dataharvester/TupleNameFilter.h>

using namespace dataharvester;
using namespace std;

TupleNameFilter::TupleNameFilter ( const string & name ) :
  theTupleName ( name ), dontCheck ( false )
{
  if ( theTupleName.empty() ) dontCheck=true;
}

bool TupleNameFilter::accepts ( const Tuple & t ) const
{
  if ( dontCheck ) return true;
  return ( t.getName() == theTupleName );
}

TupleNameFilter * TupleNameFilter::clone() const
{
  return new TupleNameFilter (*this );
}

string TupleNameFilter::name() const
{
  return "TupleNameFilter: " + theTupleName;
}
