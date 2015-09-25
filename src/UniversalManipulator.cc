#include <dataharvester/UniversalManipulator.h>

using namespace dataharvester;
using namespace std;

UniversalManipulator * UniversalManipulator::clone() const
{
  return new UniversalManipulator ( *this );
}

void UniversalManipulator::dropColumn ( const string & name )
{
  theDroppedColumns.insert ( name );
}

void UniversalManipulator::dropTuple ( const string & name )
{
  theDroppedTuples.insert ( name );
}

void UniversalManipulator::renameColumn ( const string & from, const string & to )
{
  theRenamedColumns[from]=to;
}

void UniversalManipulator::renameTuple ( const string & from, const string & to )
{
  theRenamedTuples[from]=to;
}

void UniversalManipulator::moveTuple ( const string & from, const string & to )
{
  theMovedTuples[from]=to;
}

string UniversalManipulator::getTupleName ( const string & t ) const
{
  if ( theRenamedTuples.count ( t ) )
  {
    return theRenamedTuples[t];
  }
  return t;
}

string UniversalManipulator::getColumnName ( const string & t ) const
{
  if ( theRenamedColumns.count ( t ) )
  {
    return theRenamedColumns[t];
  }
  return t;
}

void UniversalManipulator::addSimpleData ( Tuple & t, 
    const map < string, MultiType > & data,
    const string & sprefix ) const
{
  for ( map< string, MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
  {
    if ( theDroppedColumns.count ( i->first ) ) continue;
    t[sprefix+getColumnName(i->first)]=i->second;
  }
}

void UniversalManipulator::addNestedData ( Tuple & t, 
    const map < string, Tuple * > & data,
    const string & prefix ) const
{
  string sprefix(prefix);
  if ( sprefix.size() ) sprefix+=":";
  for ( map< string, Tuple * >::const_iterator i=data.begin(); i!=data.end() ; ++i )
  {
    addTuple ( t, *(i->second ), sprefix+getTupleName(i->first) );
  }
  t.fill ( prefix, false );
}

void UniversalManipulator::add ( const UniversalManipulator & o )
{
  for ( set < string >::const_iterator i=o.theDroppedColumns.begin(); 
        i!=o.theDroppedColumns.end() ; ++i )
  {
    theDroppedColumns.insert ( *i );
  }

  for ( set < string >::const_iterator i=o.theDroppedTuples.begin(); 
        i!=o.theDroppedTuples.end() ; ++i )
  {
    theDroppedTuples.insert ( *i );
  }

  for ( map < string, string >::const_iterator i=o.theRenamedColumns.begin(); 
        i!=o.theRenamedColumns.end() ; ++i )
  {
    theRenamedColumns[i->first]=i->second;
  }
  for ( map < string, string >::const_iterator i=o.theRenamedTuples.begin(); 
        i!=o.theRenamedTuples.end() ; ++i )
  {
    theRenamedTuples[i->first]=i->second;
  }
  for ( map < string, string >::const_iterator i=o.theMovedTuples.begin(); 
        i!=o.theMovedTuples.end() ; ++i )
  {
    theMovedTuples[i->first]=i->second;
  }
}

void UniversalManipulator::addTuple ( Tuple & t,
    const Tuple & orig, const string & oprefix ) const
{
  if ( theDroppedTuples.count ( orig.getName() ) ) return;
  string prefix(oprefix);
  if ( theMovedTuples.count(prefix) )
  {
    prefix=theMovedTuples[prefix];
  }
  string sprefix(prefix);
  if ( sprefix.size() ) sprefix+=":";

  vector < TupleRow > orows=orig.getTupleRows();
  for ( vector< TupleRow >::iterator i=orows.begin(); 
        i!=orows.end() ; ++i )
  {
    map < string, MultiType > simple = i->getSimpleData();
    addSimpleData ( t, simple, sprefix );

    map < string, Tuple * > nested = i->getNestedData();
    addNestedData ( t, nested, prefix );
  }

  // cout << "[UniversalManipulator] filling " << prefix << endl;
  /*
  if ( oprefix.find ( prefix ) == 0 )
  {
    // need not fill!
    return;
  }*/
  t.fill( prefix, false );
}

string UniversalManipulator::name () const
{
  return "UniversalManipulator";
  // FIXME be a bit more verbose!
}

void UniversalManipulator::manipulate ( Tuple & t ) const
{
  /*
  cout << "==-----------" << endl;
  t.dump();
  cout << "-=-----------" << endl;
  */
  Tuple newtuple ( getTupleName ( t.getName() ), t.getDescription() );
  addTuple ( newtuple, t, "" );
  t=newtuple;
  /*
  cout << "-=-----------" << endl;
  t.dump();
  cout << "==-----------" << endl;
  */
}
