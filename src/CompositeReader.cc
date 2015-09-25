#include <dataharvester/CompositeReader.h>
#include <dataharvester/TerminateException.h>
#include <sstream>

using namespace std;
using namespace dataharvester;

CompositeReader::~CompositeReader()
{
  for ( vector< AbstractReader * >::const_iterator 
        i=theSinks.begin(); i!=theSinks.end() ; ++i )
  {
    delete (*i);
  };
}

CompositeReader * CompositeReader::clone( const string & filename ) const
{
  cout << "[CompositeReader] we got a problem." << endl;
  cout << "[CompositeReader] cloning not implemented!!" << endl;
  return 0; // new CompositeReader ( filename );
}

CompositeReader::CompositeReader ( 
    const vector < AbstractReader * > & sinks )
    : theSinks ( sinks ), thePtr ( theSinks.begin() )
{
}

int CompositeReader::getNumberOfEntries() const
{
  int tot=0;
  for ( vector< AbstractReader * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    int tmp = (**i).getNumberOfEntries();
    if ( tmp < 0 ) return -1;
    tot+=tmp;
  }
  return tot;
}

std::string CompositeReader::filename() const
{
  ostringstream ostr;
  for ( vector< AbstractReader * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    if ( i!=theSinks.begin() ) ostr << ", ";
    ostr << (**i).filename();
  }
  return ostr.str();
}

void CompositeReader::setFilter ( const Filter & s )
{
  for ( vector< AbstractReader * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    (**i).setFilter ( s );
  }
}

void CompositeReader::reset()
{
  for ( vector< AbstractReader * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    (**i).reset();
  }
  thePtr=theSinks.begin();
}

Tuple CompositeReader::next()
{
  try {
    return (**thePtr).next();
  } catch ( ... ) {
    thePtr++;
    if ( thePtr == theSinks.end() )
    {
      throw dataharvester::TerminateException();
    }
    return (**thePtr).next();
  }
}

void CompositeReader::add ( AbstractReader * s )
{
  theSinks.push_back ( s );
}
