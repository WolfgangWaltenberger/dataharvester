#include <dataharvester/AbstractWriter.h>

using namespace dataharvester;
using namespace std;

AbstractWriter &                                                                                                    AbstractWriter::operator<< ( const Tuple & d )
{
  this->save( d );
  return (*this);
}

void AbstractWriter::saveFlat ( const map < string, MultiType > & data, 
      const string & name )
{
  Tuple d ( name );
  for ( map < string, MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
    d[i->first]=i->second;
  d.fill();
  save ( d );
}
