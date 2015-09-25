#include <dataharvester/HarvestingConfiguration.h>
#include "XmlWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/Mode.h>
#include <dataharvester/StringTools.h>
#include <dataharvester/CommentsExpresser.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unistd.h>

using namespace dataharvester;
using namespace std;

namespace
{
  int level ( const string & s )
  {
    if ( !s.size() ) return 0;
    int ret=1;
    string::size_type pos = s.find(":");
    while ( pos != string::npos && pos < s.size() )
    {
      pos=s.find(":",pos+1);
      ret++;
    }
    return ret;
  }

  string removeBadChars ( string src )
  {
    while ( src.find("@") != string::npos )
    {
      src.replace ( src.find("@"), 1, "_" );
    }

    while ( StringTools::find_unescaped ( src, "<" ) != string::npos )
    {
      string::size_type pos = StringTools::find_unescaped ( src, "<" );
      src.replace ( pos, 1, "[" );
    }

    while ( StringTools::find_unescaped ( src, ">" ) != string::npos )
    {
      string::size_type pos = StringTools::find_unescaped ( src, ">" );
      src.replace ( pos, 1, "]" );
    }

    return src;
  }

  string indent ( int level )
  {
    string ret="";
    for ( int i=0; i< level ; i++ ) ret+="   ";
    return ret;
  }
}

void
XmlWriter::save ( const TupleRow & row, const string & pre, const string & name )
{
  bool didPrint=false;
  string indentation = indent ( level ( pre ) );
  map< string, MultiType > sd = row.getSimpleData();
  // if ( sd.size() || row.getNestedData().size() ) // FIXME ok?
  if ( sd.size() ) // FIXME ok?
  {
    (*theStream) << indentation << "<" << name << " ";
  }

  if ( (sd.size()==0) && (row.getNestedData().size()) && ( name!="") )
  {
    (*theStream) << indentation << "<" << name << ">" << endl;
  }

  for( map< string, MultiType >::iterator mi = sd.begin();
      mi != sd.end(); ++mi )
  {
    if ( mi != sd.begin() )
    {
      (*theStream) << " ";
    }
    (*theStream) << removeBadChars ( mi->first ) << "=\""
                 << removeBadChars ( mi->second ) << "\"";
    didPrint = true;
  }
  if ( didPrint )
  {
    if ( row.getNestedData().size() )
    {
      (*theStream) << ">" << endl;
    } else {
      (*theStream) << " />" << endl;
    }
  }
  didPrint = false;

  map < string, Tuple * > nd = row.getNestedData();
  // nested data writing
  // go down for all the descendants
  for( map< string, Tuple* >::iterator i = nd.begin();
      i != nd.end(); i++)
  { // save descendant tuple
    //cout << (i->first) << " __ " << pre;
    string prefix=pre;
    if ( prefix.size() > 0 ) prefix+=":";
    prefix+= name;
    this->save( *(i->second), prefix );
    didPrint = true;
  }

  if ( nd.size() && name.size() )
  {
    (*theStream) << indentation << "</" << name << ">" << endl;
  }
}

void
XmlWriter::save ( const Tuple & d, const string & pre )
{
  CommentsExpresser exp;
  exp.manipulate ( const_cast < Tuple &> (d) );
  vector< TupleRow > dhtrs = d.getTupleRows();
  // walk down all the filled rows
  for( vector< TupleRow >::iterator ri = dhtrs.begin();
      ri != dhtrs.end(); ++ri )
  {
    save ( *ri, pre, d.getName() );
  }

  if ( !d.getCurrentRow().isEmpty() )
  {
    save ( d.getCurrentRow(), pre, d.getName() );
  }
}

void XmlWriter::setStream ( ostream & o, bool needs_deletion )
{
  if ( theStreamNeedsDeletion )
  {
    delete theStream;
  };
  theStreamNeedsDeletion=needs_deletion;
  theStream=&o;
}

XmlWriter::~XmlWriter()
{
  printFooter();
  if ( theStream && theStreamNeedsDeletion )
  {
    delete theStream;
  };
}

XmlWriter::XmlWriter ( string filename ) :
  theFileName ( filename ), theDocType("Harvest"),
  theStream ( 0 ), theStreamNeedsDeletion ( true )
{
  if ( Helpers::dontSave() ) return;
  if ( theFileName == "" ) return;
  if ( theFileName == "stdout.xml" || theFileName == "cout.xml" || theFileName == "--.xml" )
  {
    theStream = &(std::cout);
    theStreamNeedsDeletion = false;
  } else {
    ios_base::openmode mode = ofstream::app;
    if ( Mode::fileMode() == Mode::Recreate )
    {
      mode=ofstream::trunc;
    };
    setFileName();
    theStream = new ofstream ( theFileName.c_str(), mode );
    theStreamNeedsDeletion = true;
  };
  printHeader();
}

void XmlWriter::setFileName()
{
  // the xml data dataharvesterer has a special syntax:
  // filename[doctype].xml
  // this feature is realised here.
  string::size_type pos = StringTools::find_unescaped (  theFileName, "[" );
  if ( pos != string::npos )
  {
    string::size_type end = StringTools::find_unescaped (  theFileName, "]" );
    if ( end > pos && end != string::npos )
    {
      // ok, seems to work
      theDocType=theFileName.substr ( pos+1, end-pos-1 );
      theFileName=theFileName.substr ( 0,pos ) + theFileName.substr ( end+1, string::npos );
    } else {
      cout << "[XmlWriter] error: beginning [ but no end ] found. ignoring"
           << endl;
    }
  }
}

XmlWriter::XmlWriter ( ostream & o ) :
  theFileName ( "" ), theDocType("Harvest"),
  theStream ( &o ), theStreamNeedsDeletion ( false )
{}

XmlWriter *
XmlWriter::clone( const string & filename ) const
{
  return new XmlWriter ( filename );
}

void XmlWriter::printFooter() const
{
  if ( theStream ) (*theStream) << "</" << theDocType << ">" << endl;
}

void XmlWriter::printHeader() const
{
  (*theStream) << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><!DOCTYPE "
               << theDocType << ">" << endl
               << "<" << theDocType << ">" << endl;
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < XmlWriter, WriterManager::FileFormat > t ( "xml" );
}

