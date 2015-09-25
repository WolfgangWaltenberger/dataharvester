#include <dataharvester/CommentsCollector.h>
#include <utility>

using namespace dataharvester;
using namespace std;

namespace {
  static const char * tag = "__description__";

  void manipulateTuple ( Tuple & t );

  void manipulateRow ( Tuple & t, unsigned i )
  {
    TupleRow & row = t.getMutableRow ( i );

    const map < string, MultiType > & simple = 
      row.getSimpleData();

    for ( map < string, MultiType >::const_iterator i=simple.begin(); i!=simple.end() ; ++i )
    {
      string::size_type pos = i->first.find ( tag );
      if ( pos == 0 )
      {
        t.setDescription ( i->second.asString() );
        row.setDescription ( i->second.asString() );
        row.erase ( tag );
        continue;
      }

      if ( pos != string::npos )
      {
        string what=i->first.substr(0,pos);
        row.describe ( i->second.asString(), what );
        row.erase ( i->first );
      }
    }


    const map < string, Tuple * > & nested =
      row.getNestedData();

    for ( map < string, Tuple * >::const_iterator j=nested.begin(); 
          j!=nested.end() ; ++j )
    {
      manipulateTuple ( *(j->second) );
    }
  }

  void manipulateTuple ( Tuple & t )
  {
    int n = t.getNumberOfRows();

    for ( int i=0; i< n ; i++ )
      manipulateRow ( t, i );

  }
}

CommentsCollector * CommentsCollector::clone() const
{
  return new CommentsCollector ( *this );
}

string CommentsCollector::name() const
{
  return "CommentsCollector";
}

void CommentsCollector::manipulate ( Tuple & t ) const
{
  manipulateTuple ( t );
}
