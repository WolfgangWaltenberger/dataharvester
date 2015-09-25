#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Writer.h>
// #include <dataharvester/WriterManager.h>
#include <dataharvester/WriterBuilder.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/SimpleConfiguration.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace dataharvester;

namespace {
  map < string, AbstractWriter * > myWriter;
}

AbstractWriter & Writer::file( const string & s, Mode::FileMode m )
{
  Mode::setFileMode ( m );
  if ( m == Mode::Update )
  {
    SimpleConfiguration::current()->modify ("Mode:FileMode","Append");
  }
  if ( !myWriter[s] )
  {
    myWriter[s]=WriterBuilder()( s );
  };
  return ( *(myWriter[s]) );
}

void Writer::close()
{
  for ( map < string, AbstractWriter * >::const_iterator i=myWriter.begin();
        i!=myWriter.end() ; ++i )
  {
    if ( Helpers::verbose() > 10 )
    {
      cout << "[Writer] closing ``" << i->first << "'' ("
           << (void *) i->second << ")" << endl;
    };
    delete i->second;
  };
  myWriter.clear();
}

void Writer::close ( const string & filename )
{
  if ( myWriter[filename] )
  {
    delete myWriter[filename];
    myWriter[filename]=0;
  };
}

vector < string > Writer::listWriters()
{
  vector < string > ret;
  for ( map < string, AbstractWriter * >::const_iterator 
        i=myWriter.begin(); i!=myWriter.end() ; ++i )
  {
    ret.push_back ( i->first );
  };
  return ret;
}

Writer::Writer()
{
  myWriter.clear();
}

Writer::~Writer()
{
  this->close();
}
