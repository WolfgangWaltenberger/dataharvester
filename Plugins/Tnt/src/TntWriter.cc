#include <dataharvester/HarvestingConfiguration.h>
#include "TntWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/Mode.h>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unistd.h>

using namespace std;
using namespace dataharvester;

namespace
{
  string getValue ( const MultiType & m )
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
  }

  vector < char > specialChars()
  {
    static vector < char > ret;
    if ( ret.size()==0 )
    {
      /*
      ret.push_back ( ',' );
      ret.push_back ( ';' );
      ret.push_back ( ':' );
      ret.push_back ( '=' );
      */
      ret.push_back ( ' ' );
    };
    return ret;
  }

  string escapeSpecialChars ( string in, bool value_is_vector = false,
                              bool replace_with_underscore=false )
  {
    /*
    cout << "[TntWriter] replacing " << in << " " << value_is_vector << " "
         << replace_with_underscore << endl;
         */
    vector < char > scs = specialChars();
    bool had_change = false;
    do {
      had_change = false;
      for ( vector< char >::const_iterator schr=scs.begin(); 
            schr!=scs.end() ; ++schr ) // schr = special char
      {
        if ( value_is_vector && *schr == ',' ) continue;
        for ( string::iterator pos=in.begin(); pos!=in.end(); ++pos )
        {
          // cout << "pos=" << *pos << " schr=" << *schr << endl;
          if ( *pos == *schr )
          {
            if ( pos == in.begin() || *(pos-1) != '\\' )
            {
              if ( replace_with_underscore )
              {
             //   cout << "[TntWriter] replacing .. " << endl;
                in.replace( pos,pos+1,"_");
              } else {
               // cout << "[TntWriter] inserting .. " << endl;
                in.insert ( pos, 1, '\\' );
              }
              had_change=true;
              break;
            };
          };
        };
        if ( had_change ) break;
      };
    } while ( had_change );
    return in;
  }
  
  string escapeSpecialChars ( const MultiType & in,
                              bool replace_with_underscore=false )
  {
    MultiType::Type t=in.isType();
    bool is_vec = ( t == MultiType::kDouble64Vec || t == MultiType::kDouble32Vec || 
                    t == MultiType::kInt32Vec || t == MultiType::kInt64Vec );
    return escapeSpecialChars ( in.asString(), is_vec, replace_with_underscore );
  }
}

void TntWriter::save ( const TupleRow & row, const string & pre )
{
  map< string, MultiType > sd = row.getSimpleData();
  for( map< string, MultiType >::iterator mi = sd.begin();
      mi != sd.end(); ++mi )
  {
    (*theStream) << " " << escapeSpecialChars ( mi->second, true );
  }
  (*theStream) << endl;

  map < string, Tuple * > nd = row.getNestedData();
  // nested data writing
  // go down for all the descendants
  for( map< string, Tuple* >::iterator i = nd.begin(); 
      i != nd.end(); i++)
  { // save descendant tuple
    //cout << (i->first) << " __ " << pre;
    this->save( *(i->second), pre );
  }

}

void TntWriter::save ( const Tuple & d, const string & prefix )
{
  // cout << "[TntWriter] saving tuple " << d.getName() << endl;
  const_cast <Tuple &> (d).fill ( "", false );
  string pre(prefix);

  if(! pre.empty())
    pre.append(":");
  
  pre.append(d.getName());
  vector< TupleRow > dhtrs = d.getTupleRows();

  if ( theLastTuple!=pre)
  {
    (*theStream) << " " << pre << endl;
    if ( dhtrs.size() )
    {
      map< string, MultiType > sd = dhtrs[0].getSimpleData();
      //for( map< string, MultiType >::const_iterator mi = (sd).begin();
      for( map< string, MultiType >::iterator mi = sd.begin();
          mi != sd.end(); ++mi )
      {
        if ( mi !=sd.begin() ) (*theStream) << "	";
        (*theStream) << mi->first;
      }
      (*theStream) << endl;
    }
  }

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

  theLastTuple=pre;
}

void TntWriter::save ( const map < string, MultiType > & mp, 
                         const string & name )
{
  (*theStream).clear();
  if ( Helpers::dontSave() ) return;

  // name: par1=val1, par2=val2, ...
  map < string, MultiType > amp = mp;
  vector < map < string, MultiType > > comments;
  (*theStream) << " ";
  for ( map< string, MultiType >::iterator i=amp.begin();
        i!=amp.end() ; ++i )
  {
    string value = getValue ( i->second );
    bool value_is_vector = false;
    if ( i->second.isA() == "doublevec" || i->second.isA() == "intvec" ||
         i->second.isA() == "floatvec" || i->second.isA() == "int32vec" ||
         i->second.isA() == "int64vec" ||
         i->second.isA() == "boolvec" || i->second.isA() == "stringvec" )
    {
      value_is_vector = true;
    };
    (*theStream) << " " << escapeSpecialChars ( value, value_is_vector );
  };
  (*theStream) << endl;
  for ( vector< map < string, MultiType > >::const_iterator 
        i=comments.begin(); i!=comments.end() ; ++i )
  {
    save ( *i, "__ntuples__ (Meta information on the ntuples)" );
  };
  comments.clear();
  (*theStream).clear();
}

void TntWriter::setStream ( ostream & o, bool needs_deletion )
{
  if ( theStreamNeedsDeletion )
  {
    delete theStream;
  };
  theStreamNeedsDeletion=needs_deletion;
  theStream=&o;
}

TntWriter::~TntWriter()
{
  if ( theStream && theStreamNeedsDeletion )
  {
    delete theStream;
  };
}

TntWriter::TntWriter ( const string & filename ) :
  theFileName ( filename ), theStream ( 0 ), theLastTuple(""),
  theStreamNeedsDeletion ( true )
{
  if ( Helpers::dontSave() ) return;
  if ( theFileName == "" ) return;
  if ( theFileName == "--.tnt" )
  {
    theStream = &(std::cout);
    theStreamNeedsDeletion = false;
  } else {
    ios_base::openmode mode = ofstream::app;
    if ( Mode::fileMode() == Mode::Recreate )
    {
      mode=ofstream::trunc;
    };
    theStream = new ofstream ( theFileName.c_str(), mode );
    theStreamNeedsDeletion = true;
  };
}

TntWriter::TntWriter ( ostream & o ) :
  theFileName ( "" ), theStream ( &o ), theLastTuple ( ""),
  theStreamNeedsDeletion ( false )
{}

TntWriter * 
TntWriter::clone( const string & filename ) const
{
  return new TntWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < TntWriter, WriterManager::FileFormat > t ( "tnt" );
}

