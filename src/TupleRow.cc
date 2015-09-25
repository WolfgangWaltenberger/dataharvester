#include <dataharvester/TupleRow.h>
#include <dataharvester/Tuple.h>

#include <dataharvester/Helpers.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/StringTools.h>
#include <sstream>

using namespace dataharvester;
using namespace std;

namespace {
}

TupleRow::TupleRow() : simplePtr_(0), nestedPtr_(0)
{ }

TupleRow::TupleRow ( const string & name,
    const string & description ) :
  name_(name), description_(description),
  simplePtr_(0), nestedPtr_(0)
{}

// copy constructor, do the task of DEEP COPYING
TupleRow::TupleRow( const TupleRow & orig) :
  name_(orig.name_), description_ ( orig.description_),
  simpleData_ ( orig.simpleData_ ),
  columnDescrs_ ( orig.columnDescrs_ )
{
  for( map< string, Tuple* >::const_iterator i = orig.nestedData_.begin();
       i != orig.nestedData_.end(); i++)
  {
    // call the copy constructor of Tuple on the pointed-to instance
    this->nestedData_[i->first] = new Tuple( *(i->second) );
  }

  simplePtr_=simpleData_.begin();
  nestedPtr_=nestedData_.begin();
}

TupleRow & 
TupleRow::operator= ( const TupleRow & orig)
{
  this->clear();
  // FIXME
  this->simpleData_ = orig.simpleData_;
  this->columnDescrs_ = orig.columnDescrs_;
  for( map< string, Tuple* >::const_iterator i = orig.nestedData_.begin();
       i != orig.nestedData_.end(); i++)
  {
    // call the copy constructor of Tuple on the pointed-to instance
    this->nestedData_[i->first] = new Tuple( *(i->second) );
  }
  
  //return TupleRow ( *this );
  return ( *this );
}


map < string, MultiType > 
TupleRow::getSimpleData() const
{
  return simpleData_;
}

map < string, Tuple * > 
TupleRow::getNestedData() const
{
  return nestedData_;
}


TupleRow::~TupleRow()
{
  // clean up the subTuple map containing all the descendants on heap
  for( map< string, Tuple* >::iterator i = nestedData_.begin(); 
      i != nestedData_.end(); i++)
  { // delete descendant tuple
    delete (i->second);
  }
  // clear the map, just to be sure
  nestedData_.clear();
}

MultiType &
TupleRow::operator[] ( const string & name )
{
  return value( name );
}

bool
TupleRow::hasValue( const string & name) const
{ // ~= map< string, T >.has_key()
  return (simpleData_.find( name ) != simpleData_.end());
}

MultiType &
TupleRow::value( const string & name )
{
  return ( simpleData_[name] );
}


//* determine if there is an element with that name among descendants
bool
TupleRow::hasDescendant( const string & name ) const
{ // ~= map< string, T >.has_key()
  return (nestedData_.find( name ) != nestedData_.end());
}

//* give back the corresponding Tuple
Tuple &
TupleRow::descend( const string & name ) const
{ // check if pointer is set, which means no Tuple was assigned, 
  // but the key exists
  if( nestedData_[ name ] == NULL )
  {
    nestedData_[ name ] = new Tuple( name );
    if ( Helpers::verbose() > 9 )
    {
      cout << __PRETTY_FUNCTION__ << " :<" << __LINE__ << ">\n\t" 
        << "found NULL pointer to Tuple( " << name << " ) ";
      cout << "... initialised to: " << nestedData_[ name ] << endl << endl;
    };
  }

  return *(nestedData_[ name ]);
}


// checking for the emptyness of simple and nested Data maps
bool
TupleRow::isEmpty()   const  
{
  return (simpleData_.empty() && nestedData_.empty());
}

// reset to state isEmpty() == true
void
TupleRow::clear()
{
  simpleData_.clear();
  // clean up the subTuple map containing all the descendants on heap
  for( map< string, Tuple* >::iterator i = nestedData_.begin(); 
      i != nestedData_.end(); i++)
  { // delete descendant tuple
    delete (i->second);
  }
  nestedData_.clear();
}

void
TupleRow::dump( string prefix, unsigned int indent, bool type )
{
  cout << toString ( prefix, indent, type );
}
// recursively dump the tuple content, by indentation
string
TupleRow::toString( string prefix, unsigned int indent, bool type )
{
  ostringstream out;
  bool didPrint = false;
  // print all the simple values
  out << prefix;
  for( map<string, MultiType>::iterator k = simpleData_.begin();
      k != simpleData_.end(); k++)
  { // since the MultiType has defined an ovld. ostream <<...
    out << k->first << " = " << k->second;
    if ( type ) out << " (" << k->second.isA() <<")";
    out << "; \t";
    didPrint = true;
  }
  if( didPrint ) out << endl;
  didPrint = false;

  // go down for all the descendants
  for( map< string, Tuple* >::iterator i = nestedData_.begin(); 
      i != nestedData_.end(); i++)
  { // print descendant tuple
    out << (i->second)->toString(prefix, indent, type );
    didPrint = true;
  }
  if( didPrint ) out << endl;
  return out.str();
}

const TupleRow & TupleRow::__iter__() const
{
  simplePtr_=simpleData_.begin();
  nestedPtr_=nestedData_.begin();
  return (*this);
}

string TupleRow::contains() const
{
  ostringstream o;
  o << "TupleRow: ";
  if ( name_.size() )
    o << "\"" << name_ << "\"";

  if ( description_.size() )
    o << "  (" << description_ << ") ";
  o << endl;
  for ( map < string, MultiType >::const_iterator i=simpleData_.begin(); 
        i!=simpleData_.end() ; ++i )
  {
    string isa=i->second.isA();
    o << " " << i->first << "\t= ";
    if ( isa =="string" )
    {
      o << "\"" << i->second.asString() << "\"";
    } else {
      o << i->second.asString();
    };
    o << " <" << isa << ">";
    if ( columnDescrs_.count ( i->first ) )
    {
      o << "  (" << columnDescrs_[i->first] << ")";
    }
    o << endl;
  }

  o << endl;

  for ( map < string, Tuple * >::const_iterator i=nestedData_.begin(); 
        i!=nestedData_.end() ; ++i )
  {
    o << " `-- Nested tuple: " << i->first;

    if ( i->second->getDescription().size() )
      o << "  (" << i->second->getDescription() << ")";
    o << endl;
  }
  return o.str();
}

string TupleRow::getName () const
{
  return name_;
}

string TupleRow::getDescription() const
{
  return description_;
}

void TupleRow::setDescription ( const string & s )
{
  description_=s;
}

void TupleRow::setName ( const string & s )
{
  name_=s;
}

string TupleRow::getDescription ( const string & path ) const
{
  if ( path.find(":") == string::npos )
    return columnDescrs_[path];
  pair< string, string > parts = StringTools::splitPath( path );
  return descend( parts.first ).getDescription( parts.second );
}

bool TupleRow::describe ( const string & descr, const string & spath )
{
  string::size_type pos = spath.find ( ":" );
  string path=spath;
  if ( pos == path.size()-1 )
  {
    path=path.substr(0,path.size()-1);
  }

  if ( path.find(":") == string::npos )
  {
    if ( simpleData_.count ( path ) )
    {
      columnDescrs_[path]=descr;
      return true;
    }
  }

  // otherwise assume to describe a descendant, going down one hierarchy
  pair< string, string > parts = StringTools::splitPath( path );
  return descend( parts.first ).describe( descr, parts.second );
}

void TupleRow::eraseFromSimple ( const string & s )
{
  simpleData_.erase ( s );
  // simplePtr_=simpleData_.begin();
}

void TupleRow::eraseFromNested ( const string & s )
{
  nestedData_.erase ( s );
  // nestedPtr_=nestedData_.begin();
}

void TupleRow::renameSimple ( const string & from, const string & to )
{
  if ( simpleData_.find ( from ) != simpleData_.end() )
  {
    simpleData_[to]=simpleData_[from];
    simpleData_.erase ( from );
  }
}

void TupleRow::renameNested ( const string & from, const string & to )
{
  if ( nestedData_.find ( from ) != nestedData_.end() )
  {
    nestedData_[to]=nestedData_[from];
    nestedData_.erase ( from );
  }
}

void TupleRow::erase ( const string & s )
{
  simpleData_.erase ( s );
}

const string & TupleRow::next() const
{
  if ( simplePtr_ != simpleData_.end() )
  {
    map< string, MultiType >::const_iterator tmp= simplePtr_;
    simplePtr_++;
    return tmp->first;
  }

  if ( nestedPtr_ != nestedData_.end() )
  {
    map< string, Tuple * >::const_iterator tmp= nestedPtr_;
    nestedPtr_++;
    return tmp->first;
  }

  throw HarvestingException ("no more entries");
}

ostream & operator <<  ( ostream & s, TupleRow & t )
{
  s << t.contains();
  return s;
}
