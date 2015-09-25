#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/HarvestingException.h>

using namespace std;
using namespace dataharvester;

HarvestingException::HarvestingException ( const string & what ) : theWhat ( what ) {}

string HarvestingException::what() const
{
  return theWhat;
}
