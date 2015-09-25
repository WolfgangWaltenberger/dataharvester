#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/TerminateException.h>

using namespace dataharvester;
using namespace std;

TerminateException::TerminateException ( string what ) : theWhat ( what ) {}

string TerminateException::what() const
{
  return theWhat;
}
