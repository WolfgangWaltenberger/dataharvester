#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/TupleRowConsistencyChecker.h>
#include <dataharvester/Tuple.h>

using namespace std;
using namespace dataharvester;

namespace {
  bool isSubType ( MultiType::Type a, MultiType::Type b )
  {
    // check if b is subtype of a
    // (int is subtype of float ...)
    switch ( a )
    {
      case MultiType::kDouble64:
      {
        if ( b == MultiType::kDouble32 || b == MultiType::kInt64 || 
             b == MultiType::kInt32 || b == MultiType::kBool )
        {
          return true;
        } else {
          return false;
        };
        break;
      };
      case MultiType::kDouble32:
      {
        if ( b == MultiType::kInt64 || b == MultiType::kInt32 || 
             b == MultiType::kBool )
        {
          return true;
        } else {
          return false;
        };
      };
      case MultiType::kInt64:
      {
        if ( b == MultiType::kInt32 || b == MultiType::kBool )
        {
          return true;
        } else {
          return false;
        };
      };
      default:
        return false;
    };
    return false;
  }

  int verbose()
  {
    return 0;
  }
}

TupleRowConsistencyChecker::TupleRowConsistencyChecker()
{
  isDefined.clear();
}

void TupleRowConsistencyChecker::define
    ( const map < string, MultiType > & m, string tuplename )
{
  if ( isDefined[ tuplename ] )
  {
    cout << "[TupleRowConsistencyChecker] warning - second definition of \""
         << tuplename << "\"" << endl;
  };
  for ( map< string, MultiType >::const_iterator i=m.begin(); i!=m.end() ; ++i )
  {
    const_cast < MultiType & > (i->second).trimType();
    //cout <<   "[TupleRowConsistencyChecker] define " << i->first
    //     << " is a " << i->second.isA() << endl;
    theTemplate[tuplename][i->first]=i->second.isType();
  };
  isDefined[tuplename]=true;
}

bool TupleRowConsistencyChecker::check ( 
    const map < string, MultiType > & m, string tuplename )
{
  if ( m.size() == 0 ) return true;
  if (!isDefined[tuplename])
  {
    cout << "[TupleRowConsistencyChecker] Error! \"" << tuplename << "\" is not defined!" << endl;
    return false;
  };

  if ( m.size() != theTemplate[tuplename].size() )
  {
    cout << "[TupleRowConsistencyChecker] Error! Tuple layout for \"" << tuplename
         << "\" changed!" << endl;
    cout << "[TupleRowConsistencyChecker]    Was:";
    for ( map< string, MultiType::Type >::const_iterator i=theTemplate[tuplename].begin(); 
          i!=theTemplate[tuplename].end() ; ++i )
    {
      cout << " " << i->first;
    };
    cout << endl;
    cout << "[TupleRowConsistencyChecker] Is now:";
    for ( map< string, MultiType >::const_iterator i=m.begin(); i!=m.end() ; ++i )
    {
      cout << " " << i->first;
    };
    cout << endl;
    cout << "[TupleRowConsistencyChecker] Skipping tuple row!" << endl;
    return false;
  };

  map < string, bool >  filledColumn; // to check if all columns in theDates are filled
  for ( map< string, MultiType >::const_iterator i=m.begin(); i!=m.end() ; ++i )
  {
    // const_cast < MultiType & > (i->second).trimType();
    if ( i->second.isType() != theTemplate[tuplename][i->first] )
    {
      // ok - the types disagree! but this is only a problem if
      // data type is _not_ a "subset" type of the template type!
      // FIXME need to write that!
      if ( ! ( isSubType ( theTemplate[tuplename][i->first], i->second.isType() ) ))
      {
        cout << "[TupleRowConsistencyChecker] Error! Type info for column ``"
             << i->first << "'' in \""<< tuplename  << "\" inconsistent!"
             << " Was: " << i->second.typeName ( theTemplate[tuplename][i->first] )
             << " [" << theTemplate[tuplename][i->first] << "]"
             << ", is now: "  << i->second.isA() 
             << " [" << i->second.isType() << "]"
             << " (value " << i->second.asString() << ")" << endl;
        cout << "[TupleRowConsistencyChecker]  `-- Skipping data row!" << endl;
        return false;
      };
    };
    filledColumn[i->first]=true;
  };
  
  for ( map< string, MultiType::Type >::const_iterator i=theTemplate[tuplename].begin(); 
        i!=theTemplate[tuplename].end() ; ++i )
  {
    if (!filledColumn[i->first])
    {
      cout << "[TupleRowConsistencyChecker] Error! Column ``" << i->first
           << "'' is not defined in this data row!" << endl;
      cout << "[TupleRowConsistencyChecker]  `-- Skipping data row!" << endl;
      return false;
    };
  };
  return true;
}

void TupleRowConsistencyChecker::define ( const TupleRow & row, string tuplename )
{
  if ( verbose() )
  {
    cout << "[TupleRowConsistencyChecker] define " << tuplename << endl;
  };
  map < string, MultiType > simples=row.getSimpleData();
  map < string, Tuple * > nested=row.getNestedData();
  define ( simples, tuplename );
  for ( map < string, Tuple * >::const_iterator i=nested.begin(); 
        i!=nested.end() ; ++i )
  {
    string name=i->second->getName();
    vector < TupleRow > rows = i->second->getTupleRows();
    if ( ! rows.size() ) break;
    define ( rows[0], tuplename + "/" + name );
  }
}

bool TupleRowConsistencyChecker::check ( const TupleRow & row, string tuplename )
{
  map < string, MultiType > simples=row.getSimpleData();
  if (!check ( simples, tuplename ) )
  {
    return false;
  }
  map < string, Tuple * > nested=row.getNestedData();
  for ( map < string, Tuple * >::const_iterator i=nested.begin(); 
        i!=nested.end() ; ++i )
  {
    string name=i->second->getName();
    vector < TupleRow > rows = i->second->getTupleRows();
    if ( ! rows.size() ) break;
    if ( !check ( rows[0], tuplename + "/" + name ) )
    {
      return false;
    }
  }
  return true;
}
