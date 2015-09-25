#include <dataharvester/ReaderBuilder.h>
#include <dataharvester/ReaderManager.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/CompositeReader.h>

#include <iostream>
#include <sstream>

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


  AbstractReader * localReader ( const string & s )
  {
    AbstractReader * ret = 0;
    if ( s == "stdout" || s == "cout" || s == "--" )
    {
      ret =
        ReaderManager::self()->reader ("txt")->clone( s );
    } else {
      string suffix="txt";
      try {
        string::size_type pos = s.rfind(".");
        if ( pos < s.size() )
        {
          suffix = s.substr ( pos+1, s.size() );
        };
      } catch (...) {};
      AbstractReader * tmp =
        ReaderManager::self()->reader ( suffix );
      if ( !tmp )
      {
        cout << "[ReaderBuilder] suffix ``" << suffix << "'' not supported."
             << endl;
        cout << "[ReaderBuilder] storing as txt file" << endl;
        tmp =  ReaderManager::self()->reader ( "txt" );
      };
      ret = tmp->clone(s);
      if (!ret )
      {
        ostringstream o;
        o << "could not clone reader \"" << s << "\" (file does not exist?)";
        throw dataharvester::TerminateException ( o.str() );
      };
    };
    return ret;
  }

  AbstractReader * networkReader ( const string & proto, const string & file )
  {
    AbstractReader * tmp = 
      ReaderManager::self()->reader ( proto );
      // ReaderManager::self()->netReader ( proto ); // FIXME
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

  AbstractReader * singleReader( string s )
  {
    string proto = protocol ( s );
    string fname = filename ( s ); // actually this string has everything but "proto:"
    if ( proto == "file" )
    {
      // FIXME: canonized filename
      return localReader ( fname );
    }
    else
    {
      // get the right protocol reader
      return networkReader ( proto, fname );
    };
  }

}

AbstractReader * ReaderBuilder::operator()( const string & s )
{
  // if no separator is found
  if( s.find( myMulti_sink_sep ) == string::npos )
  {
    return singleReader( s );
  };
  vector < AbstractReader * > reader;
  vector < string > sinks = split ( s , ";");
  for ( vector< string >::const_iterator i=sinks.begin(); i!=sinks.end() ; ++i )
  {
    try {
      reader.push_back ( singleReader ( *i ) );
    } catch ( ... ) {
      cout << "[AbstractReader] source \"" << *i << "\" failed. Ignoring it." << endl;
    }
  };

  if ( reader.size() == 0 )
  {
    throw dataharvester::TerminateException ( "No valid source given" );
  }
  return new CompositeReader ( reader );
}

