#include "HarvestingConfiguration.h"
#ifdef WITH_SEEDER
#include "ConcreteReader.h"
#include "TerminateException.h"


using namespace harvest;
using namespace std;

ConcreteReader::~ConcreteReader()
{
}

ConcreteReader::ConcreteReader( const string & filename ) :
  theFileName(filename)
{
}

string ConcreteReader::filename() const
{
  return theFileName;
}

ConcreteReader * ConcreteReader::clone ( const string & file ) const
{
  return new ConcreteReader ( file );
}

int ConcreteReader::getNumberOfEntries() const
{
  return -1; // -1 means you dont know
}

void ConcreteReader::reset()
{
  // reset
}

Tuple ConcreteReader::next ()
{
  throw TerminateException( "no more data" );
  Tuple t;
  t.fill();
  return postProcess ( t ); // this makes sure, the filter and the manipulator work.
}

#include "ReaderPrototypeBuilder.h"

namespace {
  ReaderPrototypeBuilder<ConcreteReader> t ( "myextension" );
}

#endif /* def WITH_SEEDER */

