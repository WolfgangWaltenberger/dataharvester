#include <dataharvester/HarvestingConfiguration.h>
#include "SqliteWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/Mode.h>
#include <sstream>
#include <unistd.h>

using namespace dataharvester;
using namespace std;

namespace {
  string isA ( const string & ht )
  {
    if ( ht == "int" )
    {
      return "Int";
    }

    if ( ht == "float" || ht == "double" )
    {
      return "Float";
    }

    return "varchar";
  }
}

void SqliteWriter::error()
{
  if ( !theErr ) return;
  cout << "[SqliteWriter] Error: " << theErr << endl;
  theErr=0;
}

void
SqliteWriter::save ( const Tuple & d, const string & pre )
{
  // const_cast <Tuple &> (d).fill ( "", false );
  // your code goes here
  if ( !theDataBase ) return;

  if (!theHasTable[d.getName() ] ) 
  {
    map < string, MultiType > simple;

    if ( d.getTupleRows().size() > 0 )
    {
      simple = d.getTupleRows()[0].getSimpleData();
    } else {
      simple = d.getCurrentRow().getSimpleData();
    }

    if ( simple.size() )
    {
      ostringstream cmd;
      cmd << "create table " << d.getName() << " ( ";
      map < string, MultiType > simple = d.getTupleRows()[0].getSimpleData();
      for ( map< string, MultiType >::const_iterator i=simple.begin(); i!=simple.end() ; ++i )
      {
        if ( i->first == "name" )
        {
          cout << "[SqliteWriter] warning: column name 'name' might be problematic!" << endl;
        }
        if ( i!=simple.begin() ) cmd << ", ";
        // cout << "[SqlWriter] " << i->second.isA() << " -> " << isA ( i->second.isA() ) << endl;
        cmd << i->first << " " << isA ( i->second.isA() );
      }
      cmd << " );";
      exec ( cmd.str() );
      error();
      theHasTable[d.getName()]=true;
    }
  }

  vector < TupleRow > rows = d.getTupleRows();

  for ( vector< TupleRow >::const_iterator i=rows.begin(); i!=rows.end() ; ++i )
  {
    save ( *i, d.getName() );
  }

  if ( !d.getCurrentRow().isEmpty() )
  {
    save ( d.getCurrentRow(), d.getName() );
  }
}

void SqliteWriter::save ( const TupleRow & r, const std::string & tuple )
{
  map < string, MultiType > simple = r.getSimpleData();
  ostringstream cmd;
  cmd << "insert into " << tuple << " values (";
  for ( map < string, MultiType >::const_iterator i=simple.begin(); i!=simple.end() ; ++i )
  {
    if ( i!=simple.begin() ) cmd << ", ";
    cmd << "'" << i->second.asString() << "'";
  }
  cmd << ");";
  exec ( cmd.str() );

  map < string, Tuple * > nested = r.getNestedData();
  for ( map < string, Tuple * >::const_iterator i=nested.begin(); i!=nested.end() ; ++i )
  {
    save ( *(i->second),"" );
  }
}

void SqliteWriter::exec ( const string & cmd )
{
  // cout << "[SqliteWriter] cmd: " << cmd << endl; 
  sqlite3_exec ( theDataBase, cmd.c_str() , 0, 0, theErr ); 
}

SqliteWriter::~SqliteWriter()
{
  if ( theDataBase ) sqlite3_close ( theDataBase );
  // footers or anything?
}

SqliteWriter::SqliteWriter ( const string & filename ) :
  theFileName ( filename ), theDataBase ( 0 ), theErr ( 0 )
{
  if ( Helpers::dontSave() ) return;
  if ( dataharvester::Mode::fileMode() == dataharvester::Mode::Recreate )
  {
    unlink ( theFileName.c_str() );
  }
  if ( theFileName.size() )
    int rc = sqlite3_open ( theFileName.c_str(), &theDataBase );
}

SqliteWriter * 
SqliteWriter::clone( const string & filename ) const
{
  return new SqliteWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < SqliteWriter, WriterManager::FileFormat > t ( "sqlite" );
}

