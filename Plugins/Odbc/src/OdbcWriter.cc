#include <dataharvester/HarvestingConfiguration.h>
#if 0
#include <cstring>
#include "OdbcWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/Mode.h>
#include <odbc++/connection.h>
#include <odbc++/drivermanager.h>
#include <odbc++/databasemetadata.h>

using namespace dataharvester;
using namespace odbc;
using namespace std;

namespace {
  void listConnections()
  {
    DriverList * drivers = DriverManager::getDrivers();
    cout << "[OdbcWriter] drivers: " << endl;
    for ( DriverList::const_iterator i=drivers->begin(); 
          i!=drivers->end() ; ++i )
    {
      cout << "  " << (**i).getDescription() << endl;
    }
    cout << endl;
    
    DataSourceList * sources = DriverManager::getDataSources();
    cout << "[OdbcWriter] data sources: " << endl;
    for ( DataSourceList::const_iterator i=sources->begin(); 
          i!=sources->end() ; ++i )
    {
      cout << "  " << (**i).getDescription() << endl;
    }
    cout << endl;
  }
}

void
OdbcWriter::save ( const Tuple & d, const string & pre )
{
  // const_cast <Tuple &> (d).fill ( "", false );
  listConnections();
  openConnection();
}

void OdbcWriter::openConnection()
{
  listConnections();
  try {
    cout << "[OdbcWriter] trying to open connection to ... " << endl;
    // Connection * con = DriverManager::getConnection( "dsn=txt");
    // Connection * con = DriverManager::getConnection("test74", "postgres", "");
    // getConnection: "DSN=db;uid=user;pwd=password"
    // cout << con->getMetaData()->getDriverVersion() << endl;
  } catch ( SQLException & e ) {
    cout << e.getMessage() << endl;
  }
}

OdbcWriter::~OdbcWriter()
{
  // footers or anything?
}

OdbcWriter::OdbcWriter ( const string & filename ) :
  theFileName ( filename )
{
  if ( Helpers::dontSave() ) return;
}

OdbcWriter * 
OdbcWriter::clone( const string & filename ) const
{
  return new OdbcWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < OdbcWriter, WriterManager::NetworkProtocol > t ( "odbc" );
}

#endif
