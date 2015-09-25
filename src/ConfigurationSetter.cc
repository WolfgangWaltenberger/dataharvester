#include <dataharvester/HarvestingConfiguration.h>
#define WITH_SIMPLE_CONFIGURABLES
#ifdef WITH_SIMPLE_CONFIGURABLES
#include <dataharvester/ConfigurationSetter.h>
#include <dataharvester/SimpleConfiguration.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/Cout.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;
using namespace dataharvester;

namespace {
  vector < string > split ( string source )
  {
    vector < string > ret;
    while ( source.find(",") != string::npos )
    {
      string sub = source.substr ( 0, source.find(",") );
      if ( sub.size() ) ret.push_back ( source.substr ( 0, source.find(",") ) );
      source.replace ( 0, source.find(",")+1, "" );
      while ( source.size() && source[0]==' ' )
      {
        source.replace ( 0, 1, "" );
      };
    };
    if ( source.size() ) ret.push_back ( source );
    return ret;
  }

  pair < string, string > splitAtEqual ( string source )
  {
    pair < string, string > ret ( "", "" );
    string::size_type p = source.find("=");
    if ( p == string::npos )
      return ret;
    ret.first=source.substr(0,p);
    ret.second=source.substr(p+1,source.size());
    return ret;
  }
}

void ConfigurationSetter::set ( const string & data )
{
  vector < string > tokens = split ( data );
  if ( Helpers::verbose() )
  {
    cout << Cout::debug() << "[ConfigurationSetter] setting ";
  };
  for ( vector< string >::const_iterator i=tokens.begin(); 
        i!=tokens.end() ; ++i )
  {
    pair < string, string > nameandvalue=splitAtEqual ( *i );
    if ( Helpers::verbose() )
    {
      if (i!=tokens.begin() )
      {
        cout << ", ";
      };
      cout << nameandvalue.first << " to " << nameandvalue.second;
    };
    SimpleConfiguration::current()->modify( nameandvalue.first,
                nameandvalue.second );
  };
  if ( Helpers::verbose() )
  {
    cout << Cout::reset() << endl;
  };
}
#endif
