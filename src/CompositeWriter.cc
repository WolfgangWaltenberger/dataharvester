#include <dataharvester/CompositeWriter.h>

using namespace std;
using namespace dataharvester;

CompositeWriter::~CompositeWriter()
{
  for ( vector< AbstractWriter * >::const_iterator 
        i=theSinks.begin(); i!=theSinks.end() ; ++i )
  {
    delete (*i);
  };
}

CompositeWriter * CompositeWriter::clone( const string & filename ) const
{
  cout << "[CompositeWriter] we got a problem." << endl;
  cout << "[CompositeWriter] cloning not implemented!!" << endl;
  return 0; // new CompositeWriter ( filename );
}

CompositeWriter::CompositeWriter ( 
    const vector < AbstractWriter * > & sinks )
    : theSinks ( sinks )
{
}

void CompositeWriter::save ( const Tuple & d, const string & prefix )
{
  for ( vector< AbstractWriter * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    (**i).save ( d, prefix );
  };
}

CompositeWriter & CompositeWriter::operator<< ( const Tuple & d )
{
  for ( vector< AbstractWriter * >::const_iterator i=theSinks.begin(); 
        i!=theSinks.end() ; ++i )
  {
    (**i).operator<< ( d );
  };
  return (*this); 
}

