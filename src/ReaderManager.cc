#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/ReaderManager.h>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/StreamableReader.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/StringTools.h>
#include <dataharvester/Cout.h>
#include <algorithm>
#include <cctype>
#include <dlfcn.h>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace {
  int lower_case ( int c )
  {
    return tolower ( c );
  }

  void openLibrary ( const string & filetype )
  {
    const string dh_plugin_dir = getenv("DH_PLUGIN_DIR") ? getenv("DH_PLUGIN_DIR") : DH_PLUGIN_DIR;

    const string dlfile = dh_plugin_dir+"/"+filetype+".so";
    void * handle = dlopen ( dlfile.c_str(), RTLD_LAZY );
    if ( !handle )
    {
      cout << "[ReaderManager] could not load " << dlfile << ": " << dlerror() << endl;
      exit(1);
    }
  }
}

ReaderManager * ReaderManager::self ()
{
  if ( Helpers::verbose() > 10 )
  {
    cout << Cout::debug()
         << "[ReaderManager] ::self"
         << Cout::reset() << endl;
  };

  static ReaderManager singleton;
  return &singleton;
}

ReaderManager::ReaderManager() {}

ReaderManager::~ReaderManager()
{
  theReader.clear();
}

void ReaderManager::registerFileType (
    AbstractReader * harv, const string & filetype )
{
  list < string > filetypes=StringTools::split (filetype,"," );
  if ( Helpers::verbose() )
  {
    cout << "[ReaderManager] " << filetype << " registered." << endl;
  };
  for ( list< string >::const_iterator i=filetypes.begin(); 
        i!=filetypes.end() ; ++i )
  {
    string s = *i;
    StringTools::strip(s);
    theReader[s]=harv;
  }
}

AbstractReader * ReaderManager::reader ( const string & filetype )
{
  string lowercase=filetype;
  transform ( lowercase.begin(), lowercase.end(), lowercase.begin(), lower_case );
  if ( Helpers::verbose() > 5 )
  {
    cout << "[ReaderManager] see if " << lowercase << " is supported in static version ... ";
  };
  if ( theReader[lowercase] )
  {
      if ( Helpers::verbose() > 5 )
      cout << "yes!" << endl;
    return theReader[lowercase];
  }

  if ( Helpers::verbose() > 5 )
    cout << "no!" << endl;

  openLibrary ( lowercase );

  if ( Helpers::verbose() > 5 ) 
    cout << "[ReaderManager] " << lowercase << " plugin found!" << endl;

  return theReader[lowercase];
}

StreamableReader * ReaderManager::streamableReader ( const string & filetype )
{
  return ( dynamic_cast < StreamableReader * > ( reader ( filetype ) ) );
}


map < string, AbstractReader * > ReaderManager::supportedReaders()
{
  return theReader;
}

