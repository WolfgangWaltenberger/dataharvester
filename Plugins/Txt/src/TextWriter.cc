#include <dataharvester/HarvestingConfiguration.h>
#include "TextWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/CommentsExpresser.h>
#include <dataharvester/Mode.h>
#include <dataharvester/Cout.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

using namespace dataharvester;
using namespace std;

namespace
{
  /*
  string getValue ( const MultiType & m, int precision )
  {
    // get the value as a string.
    // make sure to add a . in case we have
    // floating point numbers
    string value = m.asString();
    if ( m.isType() == MultiType::kDouble32 || m.isType()==MultiType::kDouble64 )
    {
      if ( value.find(".") == string::npos )
      {
        value=value+".";
      };
    };
    return value;
  }*/
}

string TextWriter::quoteStrings ( const MultiType & in ) const
{
  if ( thePrecision >=0 &&(  in.isA() == "double" || in.isA() == "float" ) )
  {
    ostringstream ostr;
    ostr << setprecision ( thePrecision ) << fixed << in.asDouble();
    return ostr.str();
  }
  string ret = in.asString();
  // remove carriage returns!
  while ( ret.find("\n") != string::npos )
  {
    ret=ret.replace(ret.find("\n"),1,"");
  }

  if ( in.isA() == "string" )
  { 
    ret = "\"" + ret + "\"";
    if (theUseColors ) 
    {
      ret=Cout::warning(true) + ret + Cout::reset( true );
    }
  }
  // ret += " (" + in.isA() + ")";
  return ret;
}

void
TextWriter::save ( const TupleRow & row, const string & pre)
{
  bool use_short_notation=true;
  bool didPrint = false;

  // cannot do short notation with nested data
  if ( row.getNestedData().size() ) use_short_notation=false;

  // cout << " -- writing " << theUseColors << endl;

  map< string, MultiType > sd = row.getSimpleData();
  if ( sd.size() )
  {
    if ( theUseColors ) (*theStream) << Cout::error( true );
    (*theStream) << pre << ": ";
    if ( theUseColors ) (*theStream) << Cout::reset( true );
    map< string, MultiType >::iterator end = sd.end();
    end--;

    for( map< string, MultiType >::iterator mi = sd.begin();
        mi != sd.end(); ++mi )
    { // use the ovld MultiType <<
      if ( mi != sd.begin() ) 
      {
        (*theStream) << " ";
      }
      if ( theUseColors ) (*theStream) << Cout::info( true );
      (*theStream) << mi->first << "=";
      if ( theUseColors ) (*theStream) << Cout::warning( true );
      (*theStream) << quoteStrings ( mi->second );
      if ( theUseColors ) (*theStream) << Cout::reset( true );
      if ( (!use_short_notation) || mi!=end )
      {
        (*theStream) << ";";
      } else {
        if ( theUseColors ) (*theStream) << Cout::error( true );
        (*theStream) << ".";
        if ( theUseColors ) (*theStream) << Cout::reset( true );
      }
      didPrint = true;
    }
  }
  if( didPrint ) 
    (*theStream) << endl;
  didPrint = false;

  map < string, Tuple * > nd = row.getNestedData();
  // nested data writing
  // go down for all the descendants
  for( map< string, Tuple* >::iterator i = nd.begin(); 
      i != nd.end(); i++)
  { // save descendant tuple
    //cout << (i->first) << " __ " << pre;
    this->save( *(i->second), pre );
    didPrint = true;
  }
  if (!use_short_notation )
  {
    if ( theUseColors ) (*theStream) << Cout::error( true );
    (*theStream) << pre << ":fill";
    if ( theUseColors ) (*theStream) << Cout::reset( true );
    (*theStream) << endl;
  }
}

void
TextWriter::save ( const Tuple & d, const string & prefix)
{
  // const_cast <Tuple &> (d).fill ( "", false );
  CommentsExpresser exp;
  exp.manipulate ( const_cast < Tuple &> (d) );
  string pre( prefix );
  if(! pre.empty())
    pre.append(":");
  
  pre.append(d.getName());
  
  vector< TupleRow > dhtrs = d.getTupleRows();
  // walk down all the filled rows
  for( vector< TupleRow >::iterator ri = dhtrs.begin();
      ri != dhtrs.end(); ++ri )
  {
    save ( *ri, pre );
  }

  if ( !d.getCurrentRow().isEmpty() )
  {
    save ( d.getCurrentRow(), pre );
  }
}

void TextWriter::setStream ( ostream & o, bool needs_deletion )
{
  if ( theStreamNeedsDeletion )
  {
    delete theStream;
  };
  theStreamNeedsDeletion=needs_deletion;
  theStream=&o;
}

TextWriter::~TextWriter()
{
  if ( theStream && theStreamNeedsDeletion )
  {
    delete theStream;
  };
}

TextWriter::TextWriter ( const string & filename ) :
  theFileName ( filename ), theStream ( 0 ), theStreamNeedsDeletion ( true ),
  theUseColors ( false ), thePrecision ( -1 )
{
  if ( Helpers::dontSave() ) return;
  if ( theFileName == "" ) return;
  if ( theFileName.substr(0,6) == "stdout" ||
       theFileName.substr(0,8) == "stdcolor" ||
       theFileName.substr(0,4) == "cout" || theFileName.substr(0,2) == "--" )
  {
    setStream ( cout, false );
    if ( theFileName.substr(0,8) == "stdcolor" ) theUseColors=true;  
    string::size_type pos = theFileName.find("_precision");
    if ( pos != string::npos && pos != theFileName.size() )
    {
      int p = atoi ( theFileName.substr( pos+10, 1 ).c_str() );
      if ( p>= 0 ) thePrecision=p;
    }
    /*
    theStream = &(cout);
    theStreamNeedsDeletion = false;
    */
  } else {
    ios_base::openmode mode = ofstream::app;
    if ( Mode::fileMode() == Mode::Recreate )
    {
      mode=ofstream::trunc;
    };
    setStream ( *(new ofstream ( theFileName.c_str(), mode )), true );
    /*
    theStream = new ofstream ( theFileName.c_str(), mode );
    theStreamNeedsDeletion = true;
    */
  };
}

TextWriter::TextWriter ( ostream & o ) :
  theFileName ( "" ), theStream ( &o ), theStreamNeedsDeletion ( false ),
  theUseColors ( false ), thePrecision ( -1 )
{}

TextWriter * 
TextWriter::clone( const string & filename ) const
{
  return new TextWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < TextWriter, WriterManager::FileFormat > t ( "txt" );
}

