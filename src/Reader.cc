#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Reader.h>
#include <dataharvester/ReaderManager.h>
#include <dataharvester/ReaderBuilder.h>
#include <dataharvester/HarvestingException.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace dataharvester;
using namespace std;

namespace {
  map < string, AbstractReader * > myReader;
  int verbose()
  {
    return 0;
  }
}

AbstractReader & Reader::file( const string & s )
{
  if ( verbose() > 0 )
  {
    cout << "[Reader] produce the seeder" << endl;
  };
  if ( !myReader[s] )
  {
    myReader[s]=ReaderBuilder()(s);
  };
  return ( *(myReader[s] ) );

  /*
  string filename = s;
  string::size_type pos = filename.rfind("/");
  if ( pos < filename.size() )
  {
    filename=filename.substr ( pos+1, s.size() );
  };
  if (!myReader[s])
  {
    if ( s == "stdin" || s == "cin" || s == "--" )
    {
      AbstractReader * tmp =
        ReaderManager::self()->seeder ("txt");
      if (!tmp)
      {
        cout << "[Reader] concrete seeder ``" << s << "''"
             << " cannot be cloned!" << endl;
        exit(-1);
      };
      myReader[s]=tmp->clone(s);
    } else {
      string suffix="txt";
      try {
        string::size_type pos = filename.rfind(".");
        if ( pos < filename.size() )
        {
          suffix = filename.substr ( pos+1, filename.size() );
        };
      } catch (...) {};
      AbstractReader * tmp =
        ReaderManager::self()->seeder ( suffix );
      if ( !tmp )
      {
        ostringstream ostr;
        ostr << "[Reader] suffix ``" << suffix << "'' not supported.";
        cout << ostr.str() << endl;
        throw HarvestingException ( ostr.str() );
        // exit(-1);
        // return 0;
      };
      myReader[s] = tmp->clone ( s );
    };
  };
  if ( verbose() > 0 )
  {
    cout << "[Reader] seeder produced" << endl;
  };
  if (!myReader[s] )
  {
    cout << "[Reader] error: could not clone concrete seeder: ``"
         << s << "'' (badly chosen file name?)" << endl;
    throw HarvestingException("could not clone seeder");
  };
  return (*(myReader[s]));
  */
}

void Reader::close( const string & s )
{
  if ( myReader[s] )
  {
    delete myReader[s];
    myReader[s]=0;
  }
}

Reader::Reader()
{
  myReader.clear();
}

Reader::~Reader()
{
  for ( map < string, AbstractReader * >::const_iterator 
        i=myReader.begin(); i!=myReader.end() ; ++i )
  {
    delete i->second;
  };
  myReader.clear();
}

