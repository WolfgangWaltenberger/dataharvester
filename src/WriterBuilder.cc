#include <dataharvester/WriterBuilder.h>
#include <dataharvester/WriterManager.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/CompositeWriter.h>

#include <iostream>
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace {

  // the separator for multiple output destinations
  static const string myMulti_sink_sep(";");
  static const string::size_type  myMulti_sink_sep_size =
                                        myMulti_sink_sep.size();

  // tool for returning position of unescaped substring in src string
  // FIXME: there's a BUG IN HERE...
  //    corrected in the new class RLCanonizer
  string::size_type
    find_unescaped( const string & src, const string & sub )
    {
      string::size_type pos;
      // walk the string WHILE finding escaped....
      do
      {
         pos = src.find( sub );
         //cout << "[find_unescaped]: found pos " << pos << endl;
      }
      while( pos != string::npos && pos > 0 && src[pos-1] == '\\' );

      return pos;
    }


  AbstractWriter * localWriter ( string s )
  {
    AbstractWriter * ret = 0;
    if ( s.substr(0,6) == "stdout" || s.substr(0,8) == "stdcolor" || s.substr(0,4) == "cout" || s.substr(0,2) == "--" )
    {
      ret =
        WriterManager::self()->writer ("txt")->clone( s );
    } else {
      string suffix="txt";
      try {
        string::size_type pos = s.rfind(".");
        if ( pos < s.size() )
        {
          suffix = s.substr ( pos+1, s.size() );
        };
      } catch (...) {};
      AbstractWriter * tmp =
        WriterManager::self()->writer ( suffix );
      if ( !tmp )
      {
        cout << "[WriterBuilder] suffix ``" << suffix << "'' not supported."
             << endl;
        cout << "[WriterBuilder] storing as txt file" << endl;
        tmp =  WriterManager::self()->writer ( "txt" );
      };
      ret = tmp->clone(s);
      if (!ret )
      {
        cout << "[WriterBuilder] error: could not clone concrete writer: ``"
             << s << "'' (badly chosen file name?)" << endl;
        exit (-1);
      };
    };
    return ret;
  }

  AbstractWriter * networkWriter ( string proto, string file )
  {
    AbstractWriter * tmp = 
      WriterManager::self()->netWriter ( proto );
    if ( tmp )
    {
      return tmp->clone(file);
    } else {
      throw HarvestingException ((string) "protocol " + proto + " not supported" );
    };
  }

  string protocol ( string id )
  {
    string ret = "file";
    string::size_type pos = id.find(":");
    if ( (signed) pos != -1 && id[pos-1]!='\\' )
    {
      ret = id.substr(0,id.find(":"));
    };
    return ret;
  }

  string filename ( string id )
  {
    string::size_type pos = id.find(":");
    string ret = id;
    if ( (signed) pos != -1 && id[pos-1]!='\\' )
    {
      ret = id.substr(id.find(":")+1);
    };
    return ret;
  }

  vector< string > 
    split( string source, string sepstr )
  {
    vector< string > ret;
    string::size_type foundpos;

    while( (foundpos = find_unescaped( source, myMulti_sink_sep )) != 
        string::npos )
    {
      string sub = source.substr( 0, foundpos );
      if( sub.size() ) 
        ret.push_back( source.substr ( 0, foundpos ) );
      source.replace( 0, foundpos + myMulti_sink_sep_size, "" );
      // eat up leading spaces (~= pythons string.lstrip())
      // TODO: separate function? something from the STL/other lib?
      while( source.size() && source[0]==' ' )
      { source.replace ( 0, 1, "" ); };
    };
    if( source.size() ) 
      ret.push_back( source );

    return ret;
  }

  AbstractWriter * singleWriter( string s )
  {
    string proto = protocol ( s );
    string fname = filename ( s ); // actually this string has everything but "proto:"
    if ( proto == "file" )
    {
      // FIXME: canonized filename
      return localWriter ( fname );
    }
    else
    {
      // get the right protocol writer
      return networkWriter ( proto, fname );
    };
  }

}

AbstractWriter * WriterBuilder::operator()( string s )
{
  // if no separator is found
  if( s.find( myMulti_sink_sep ) == string::npos )
  {
    return singleWriter( s );
  };
  vector < AbstractWriter * > writer;
  vector < string > sinks = split ( s , ";");
  for ( vector< string >::const_iterator i=sinks.begin(); i!=sinks.end() ; ++i )
  {
    writer.push_back ( singleWriter ( *i ) );
  };

  return new CompositeWriter ( writer );
}

