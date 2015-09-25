#include <dataharvester/Reader.h>
#include <dataharvester/Tuple.h>
#include <dataharvester/TerminateException.h>
#include <iostream>

int main()
{
  using namespace dataharvester;
  using namespace std;
  try {
    while ( true )
    {
      Tuple tuple = Reader::file ( "my.txt" ).next();
 
      // retrieve first (and, often, only) occurence of the variable "age",
      // knowing that it's a float (implicitly casting from a MultiType)
      float age = tuple.getFirst ( "age" );

      // same thing, returning it as a string
      // (also an implicit cast)
      string s_age = tuple.getFirst ( "age" );

      // get all tuple rows
      vector < TupleRow > rows = tuple.getTupleRows();
  
      for ( vector< TupleRow >::const_iterator row=rows.begin(); row!=rows.end() ; ++row )
      {
        // the "flat data" in every tuple row
        map < string, MultiType > data = row->getSimpleData();
        for ( map < string, MultiType >::const_iterator d=data.begin(); d!=data.end(); ++d )
        {
          cout << d->first << "=" << d->second.asString() << endl;
        }

        // the "nested data" in every tuple row
        map < string, Tuple * > nested = row->getNestedData();
      }
    }
  } catch ( TerminateException & ) {};
}
