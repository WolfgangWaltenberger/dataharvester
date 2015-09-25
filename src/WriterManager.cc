#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/WriterManager.h>
#include <dataharvester/AbstractWriter.h>
#include <dataharvester/StreamableWriter.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/StringTools.h>
#include <dlfcn.h>
#include <algorithm>
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
      cout << "[WriterManager] could not load " << dlfile << ": " << dlerror() << endl;
      exit(1);
    }
  }
}

WriterManager * WriterManager::self ()
{
  if ( Helpers::verbose() > 99 )
  {
    cout << "[WriterManager] ::self" << endl;
  };
  static WriterManager singleton;
  return &singleton;
}

WriterManager::WriterManager() {}

WriterManager::~WriterManager()
{
  // could delete them, but why?
  // The OS is more efficient.
  theWriter.clear();
}

void WriterManager::registerFileType (
    AbstractWriter * harv, const string & filetype )
{
  list < string > filetypes=StringTools::split (filetype,"," );
  if ( Helpers::verbose() )
  {
    cout << "[WriterManager] filetype " << filetype << " registered." << endl;
  };
  for ( list< string >::const_iterator i=filetypes.begin(); 
        i!=filetypes.end() ; ++i )
  {
    theWriter[*i]=harv;
  };
}

void WriterManager::registerProtocol (
    AbstractWriter * harv, const string & proto )
{
  list < string > protos=StringTools::split(proto,",");
  if ( Helpers::verbose() )
  {
    cout << "[WriterManager] protocol " << proto << " registered." << endl;
  };
  for ( list< string >::const_iterator i=protos.begin(); 
        i!=protos.end() ; ++i )
  {
    theNetWriter[*i]=harv;
  };
}

void WriterManager::registers (
    int type, AbstractWriter * harv, const string & s )
{
  switch ( type )
  {
    case FileFormat:
      registerFileType ( harv, s );
      break;
    case NetworkProtocol:
      registerProtocol ( harv, s );
      break;
    default:
    {
      cout << "[WriterManager] error! Trying to register a type "
           << type << "!" << endl;
      exit(-1);
    };
  };
}

AbstractWriter * WriterManager::writer ( 
    const string & filetype )
{
  string lowercase=filetype;
  transform ( lowercase.begin(), lowercase.end(), lowercase.begin(), lower_case );
  if ( Helpers::verbose() > 5 )
  {
    cout << "[WriterManager] see if " << lowercase << " is supported in static version ... ";
  };
  if ( theWriter[lowercase] )
  {
    if ( Helpers::verbose() > 5 )
      cout << "yes!" << endl;
    return theWriter[lowercase];
  }

  if ( Helpers::verbose() > 5 ) cout << "no!" << endl;

  openLibrary ( lowercase );

  if ( Helpers::verbose() > 5 ) 
    cout << "[WriterManager] " << lowercase << " plugin found!" << endl;

  return theWriter[lowercase];
}

AbstractWriter * WriterManager::netWriter ( 
    const string & protocol )
{
  string lowercase=protocol;
  transform ( lowercase.begin(), lowercase.end(), lowercase.begin(), lower_case );
  if ( Helpers::verbose() )
  {
    cout << "[WriterManager] see if " << lowercase << " is supported in static version ... ";
  };
  if ( theNetWriter[lowercase] )
  {
    if ( Helpers::verbose() )
      cout << "yes!" << endl;
    return theWriter[lowercase];
  }

  if ( Helpers::verbose() ) cout << "no!" << endl;

  openLibrary ( lowercase );

  if ( Helpers::verbose() ) 
    cout << "[WriterManager] plugin found!" << endl;

  return theNetWriter[lowercase];
}

StreamableWriter * WriterManager::streamableWriter ( 
    const string & filetype )
{
  return ( dynamic_cast <StreamableWriter *> (writer(filetype) ) );
}

