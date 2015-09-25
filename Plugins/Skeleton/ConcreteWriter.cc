#include "HarvestingConfiguration.h"
#ifdef WITH_HARVESTER
#include "ConcreteWriter.h"
#include "Helpers.h"
#include "Mode.h"

using namespace harvest;


void
ConcreteWriter::save ( const Tuple & d, const string & pre )
{
  const_cast <Tuple &> (d).fill ( "", false );
  // your code goes here
}

ConcreteWriter::~ConcreteWriter()
{
  // footers or anything?
}

ConcreteWriter::ConcreteWriter ( const string & filename ) :
  theFileName ( filename )
{
  if ( Helpers::dontSave() ) return;
}

ConcreteWriter * 
ConcreteWriter::clone( const string & filename ) const
{
  return new ConcreteWriter ( filename );
}

#include "WriterPrototypeBuilder.h"
namespace {
  WriterPrototypeBuilder
    < ConcreteWriter, WriterManager::FileFormat > t ( "myextension" );
}

#endif /* def WITH_HARVESTER */
