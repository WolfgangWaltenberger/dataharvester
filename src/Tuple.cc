#include <dataharvester/Tuple.h>
#include <dataharvester/TupleRow.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/StringTools.h>
#include <string>
#include <utility>
#include <sstream>

using namespace std;
using namespace dataharvester;
using namespace StringTools;

Tuple::Tuple() : tname_(""), tdescr_(""), rowctr_(0)
{ }

Tuple::Tuple( const char *name, const char *comment ) :
  tname_( name ), tdescr_( comment ), rowctr_(0)
{ 
  curRow_.setName ( tname_ ); // rows take on the names of the tuples.
  curRow_.setDescription ( tdescr_ );
}

Tuple::Tuple( const string & name, const string & comment ) :
  tname_( name ), tdescr_( comment ), rowctr_(0)
{
  curRow_.setName ( tname_ ); // rows take on the names of the tuples.
  curRow_.setDescription ( tdescr_ );
}

Tuple::~Tuple()
{
  this->clear();
}

const TupleRow & Tuple::getCurrentRow() const
{
  return curRow_;
}
 
string Tuple::contains() const
{
  ostringstream o;
  o << "Tuple: \"" << tname_ << "\"";
  if ( tdescr_.size() )
    o << "  (" << tdescr_ << ")";
  o << endl;
  o << "   " << tupRows_.size() << " tuple row";
  if ( tupRows_.size() != 1 ) o << "s";
  o << ", pointer is at " << rowctr_ << endl;

  if ( tupRows_.size() )
  {
    o << endl << "First row: ";
    o << getRow(0).contains();
  }
  
  if ( !(curRow_.isEmpty())) {
    o << endl << "Current row: ";
    o << curRow_.contains();
  }

  return o.str();
}

void Tuple::clear()
{ // actually there is nothing to do... all data members are instances
  // but let us be nice:
  tupRows_.clear();
}

/*
// syntactically shallow value copy
// although there is no shallow data members, SO: DEEP COPY
Tuple &
Tuple::operator= ( const Tuple & rhs)
{
  this->tname_ =     rhs.tname_;
  this->tdescr_ =  rhs.tdescr_;
  this->curRow_ =    rhs.curRow_;
  this->tupRows_ =   rhs.tupRows_;

  return(*this);
}*/

// commit the current row to the vector container
bool
Tuple::fill( const char *path, bool warn )
{
  return fill( string(path), warn );
}

string Tuple::getName() const
{
  return tname_;
}

string Tuple::getDescription() const
{
  return tdescr_;
}

string Tuple::getDescription ( const string & column ) const
{
  if ( column == (string) "" ) return tdescr_;
  return curRow_.getDescription ( column );
}

void Tuple::setName( const string & name )
{
  tname_=name;
  /*
  curRow_.setName ( name );
  for ( int i=0; i< getNumberOfRows() ; i++ )
  {
    tupRows_[i].setName( name );
  }*/
}

void Tuple::setDescription ( const string & d )
{
  tdescr_=d;
  // this should be the same for all.
  /*
  curRow_.setDescription ( d );
  for ( int i=0; i< getNumberOfRows() ; i++ )
  {
    tupRows_[i].setDescription ( d );
  }*/
}

// commit the current row to the vector container
bool Tuple::fill( const string & path, bool warn )
{
  // check for fill with empty string, which means:
  // do start new row here, on this hierarchy
  if ( path == (string) "" )
  {
    if ( curRow_.isEmpty() )
    {
      if ( warn )
        cout << __PRETTY_FUNCTION__ << ":" << __LINE__ << " "
          << "(\"" << tname_ << "\" )\n\t"
          << "** WARNING **: trying to append an empty row in "
              << tname_ << " !\n"
          << "\t... Skipping!" << endl << endl;
      return false;
    };

    curRow_.setName ( tname_ ); // rows take on the names of the tuples.
    curRow_.setDescription ( tdescr_ );
    tupRows_.push_back( curRow_ );
    curRow_.clear();
  }
  // otherwise assume to fill a descendant, going down one hierarchy
  else
  {
    pair< string, string > parts = StringTools::splitPath( path );
    curRow_.descend( parts.first ).fill( parts.second, warn );
  };

  return true;
}

void Tuple::add (const TupleRow & r )
{
  tupRows_.push_back ( r );
}

MultiType & Tuple::operator[] ( const char * path )
{
  return operator[]( string(path) );
}

bool Tuple::hasDescendantInFirstRow ( const string & row ) const
{
  if ( tupRows_.size() == 0 ) return false;
  return tupRows_[0].hasDescendant ( row );
}

Tuple & Tuple::descendInFirstRow ( const string & name )
{
  if ( tupRows_.size() == 0 ) throw HarvestingException("cannot descend");
  return tupRows_[0].descend ( name );
}

bool Tuple::hasValueInFirstRow ( const string & row ) const
{
  if ( tupRows_.size() == 0 ) return false;
  return tupRows_[0].hasValue ( row );
}

vector < MultiType > Tuple::getAll ( const char * path )
{
  vector < MultiType > ret;
  for ( vector< TupleRow >::iterator i=tupRows_.begin();
        i!=tupRows_.end() ; ++i )
  {
    ret.push_back ( (*i)[path] );
  }
  return ret;
}

MultiType Tuple::getFirst ( const char * path )
{
  if ( tupRows_.size() )
  {
    return tupRows_[0][path];
  } else {
    return MultiType();
  }
}

MultiType & Tuple::operator[] ( const string & path )
{
  // TODO: strip the whitespaces in front and back ???
  //    (time consuming nanny's job)

  // if there is a hierarchy separator (":", HS) go dig for a tuple
  //    e.g.: "event:info:id"
  string::size_type pos = find_unescaped( path, string(":"));
  if( pos != string::npos )
  {

// TODO: shit-check of head & tail ("", whatever...)
    string head( path.substr( 0, pos ));  // first part of path "event"
    string tail( path.substr( pos+1 ));     // tail part of path "info:id"

    // cout << "[Tuple:" << tname_ << " got head: " << head << endl;
    // cout << "[Tuple:" << tname_ << " got tail: " << tail << endl;

    /*  FIXME
    // check existence of head-name Tuple
    if( ! curRow_.hasDescendant( head ) )    // ~= not has_key()
    {
      // - or create first
      curRow_.descend( head ) = Tuple( head );
      // cout << "[Tuple] creating tuple >" << head << "<" << endl;
      // cout << "[Tuple] tuplename is " << subTuple[head].getname_() << endl;
    }
    */

    // cout << "[Tuple] filling >" << tail << "<" << endl;
    // then fill... by delegating to lower hierarchy.
    return curRow_.descend( head ).operator[]( tail );

  }
  // we have unseparated string as identifier.
  else
  {
    //  last vector entry has_key()
    if( curRow_.hasValue( path ) )
    {   // which means we've been here before, and won't overwrite
      /*
      cout << __PRETTY_FUNCTION__ << " :<" << __LINE__ << "> "
        << "( " << tname_ << " )\n\t"
        << "** WARNING **: column \"" << path << "\" "
            << "has been filled before !!!" << endl
        << "\tnew value overwriting old value." << endl << endl;
        */
    }

    return curRow_[ path ];
  }

  // shouldn't get here... RAISE EXCEPTION ?
  cout << "Tuple::fill(); " << __LINE__ << "shouldn't get here !!!" << endl;
  throw HarvestingException ("Tuple::fill(); shouldn't get here !!!" );
}

bool Tuple::hasValue ( const char * v ) const
{
  return curRow_.hasValue ( v );
}

bool Tuple::describe( const char *descr, const char *path )
{
  describe( string(descr), string(path) );
  return true;
}

// this works like fill(), descending down the hierarchy...
bool
Tuple::describe( const string & descr, const string & path )
{
  // check for fill with empty string, which means:
  // do set the description here, on this hierarchy
  if ( path == (string) "" )
  {
    tdescr_ = string( descr );
  } else {
    return curRow_.describe ( descr, path );
  }
  return true;
}

vector < TupleRow > Tuple::getTupleRows() const
{
  return tupRows_;
}

unsigned Tuple::getNumberOfRows() const
{
  return tupRows_.size();
}

void
Tuple::dump( string prefix, unsigned int indent, bool type )
{
  cout << toString ( prefix, indent, type );
}

// Recursively dump the tuple content, by indentation
// toString is conceived for usage with dump,
// for other usages ::contains should be the more
// adaequate method.
string Tuple::toString( string prefix, unsigned int indent, bool type )
{
  ostringstream out;
  string ind("+");
  ind.append(indent-2,'-');
  ind.append("> ");

  // print a header
  out << prefix << ind << tname_ << " : \t\"" << tdescr_ << "\"" << endl;
  // increase indentation
  prefix.append(indent, ' ');

  // print all the tuple rows
  for( vector< TupleRow >::iterator i = tupRows_.begin();
      i != tupRows_.end(); i++)
  { // print one tuple row
    out << i->toString(prefix, indent, type );
  }

  // dump current row
  if ( ! curRow_.isEmpty() )
  {
    out << prefix << "* current row:";
    out << curRow_.toString( prefix, indent, type );
  };
  return out.str();
}

void Tuple::add ( const string & name, const MultiType & value )
{
  operator[](name)=value;
}

const Tuple & Tuple::__iter__() const
{
  rowctr_=0;
  return (*this);
}

const TupleRow & Tuple::getRow ( unsigned idx ) const
{
  if ( tupRows_.size() > idx )
  {
    return tupRows_[idx];
  }
  throw HarvestingException ("Row does not exist");
}

TupleRow & Tuple::getMutableRow ( unsigned idx )
{
  if ( tupRows_.size() > idx )
  {
    return tupRows_[idx];
  }
  throw HarvestingException ("Row does not exist");
}

const TupleRow & Tuple::next() const
{
  if ( tupRows_.size() > rowctr_ )
  {
    return tupRows_[rowctr_++];
  }
  throw HarvestingException ("no more rows");
}

ostream & operator <<  ( ostream & s, Tuple & t )
{
  s << t.contains();
  return s;
}
