#include <dataharvester/AbstractReader.h>
#include <dataharvester/NoFilter.h>
#include <dataharvester/NoManipulator.h>
#include <sstream>

using namespace dataharvester;
using namespace std;

AbstractReader::~AbstractReader()
{
  delete theFilter;
  delete theManipulator;
}

AbstractReader::AbstractReader () : theFilter ( new NoFilter() ),
  theManipulator ( new NoManipulator() )
{}

AbstractReader::AbstractReader ( const AbstractReader & o ) :
  theFilter ( o.theFilter->clone() ),
  theManipulator ( o.theManipulator->clone() )
{}

Tuple AbstractReader::postProcess ( Tuple & t )
{
  theManipulator->manipulate ( t );
  if ( !theFilter->accepts ( t ) )
  {
    return next();
  }
  return t;
}

void AbstractReader::setManipulator ( const Manipulator & m )
{
  theManipulator=m.clone();
};

void AbstractReader::setFilter ( const Filter & f )
{
  theFilter=f.clone();
}

string AbstractReader::describe() const
{
  ostringstream o;
  o << "Reader: \"" << filename() << "\"" << endl;
  o << "    `-      Filter: " << theFilter->name() << endl;
  o << "    `- Manipulator: " << theManipulator->name() << endl;
  return o.str();
}

ostream & operator <<  ( ostream & s, AbstractReader & reader )
{
  s << reader.describe();
  return s;
}
