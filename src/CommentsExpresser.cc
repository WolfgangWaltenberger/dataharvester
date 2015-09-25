#include <dataharvester/CommentsExpresser.h>
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
      if ( t.getDescription ( i->first ).size() )
      {
        row[i->first+tag]=t.getDescription ( i->first );
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
    bool fill=false;

    if ( t.getDescription().size() )
    {
      fill=true;
      // we have a description for this tuple.
      for ( int i=0; i< n ; i++ )
        t.getMutableRow(i)[ tag ]=t.getDescription();
    }

    for ( unsigned i=0; i< n ; i++ )
      manipulateRow ( t, i );

    // if ( fill ) t.fill ( "__description__" );
  }
}

CommentsExpresser * CommentsExpresser::clone() const
{
  return new CommentsExpresser ( *this );
}

string CommentsExpresser::name() const
{
  return "CommentsExpresser";
}

void CommentsExpresser::manipulate ( Tuple & t ) const
{
  manipulateTuple ( t );
}
