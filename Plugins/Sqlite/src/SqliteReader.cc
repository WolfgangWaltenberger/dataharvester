#include <dataharvester/HarvestingConfiguration.h>
#include "SqliteReader.h"
#include <dataharvester/TerminateException.h>
#include <sstream>
#include <sqlite3.h>


using namespace dataharvester;
using namespace std;

namespace dataharvester {
  class DataBase {
  private:
    sqlite3 *db;
    char *zErrMsg;
    char **result;
    int rc;
    int nrow,ncol;
    int db_open;
    char ** err;

  public:

    std::vector<std::string> vcol_head;
    std::vector<std::string> vdata;

    DataBase ( const std::string & tablename="init.db"): zErrMsg(0), rc(0),db_open(0)
    {
      int rc = sqlite3_open ( tablename.c_str(), &db );
      // db = sqlite3_open(tablename.c_str(), 0, err );
      if( rc ){
        cout << "[SqliteReader] cant open database." << endl;
        //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
      }
      db_open=1;
    }

    int exe( const std::string & s_exe )
    {
        rc = sqlite3_get_table(
                        db,              /* An open database */
                        s_exe.c_str(),       /* SQL to be executed */
                        &result,       /* Result written to a char *[]  that this points to */
                        &nrow,             /* Number of result rows written here */
                        &ncol,          /* Number of result columns written here */
                        &zErrMsg          /* Error msg written here */
                        );

        if(vcol_head.size()<0) { vcol_head.clear();  }
        if(vdata.size()<0)     { vdata.clear(); }

       if( rc == SQLITE_OK ){
        for(int i=0; i < ncol; ++i)
        vcol_head.push_back(result[i]);   /* First row heading */
        for(int i=0; i < ncol*nrow; ++i)
        vdata.push_back(result[ncol+i]);
       }
       sqlite3_free_table(result);
       return rc;
    }

    ~DataBase(){
        sqlite3_close(db);
    }
  };

}

void SqliteReader::error()
{
  /*
  if ( !theErr ) return;
  cout << "[SqliteReader] Error: " << theErr << endl;
  theErr=0;
  */
}


SqliteReader::~SqliteReader()
{
  if ( theDataBase )
  {
    delete theDataBase;
  }
}

SqliteReader::SqliteReader( const string & filename ) :
  theFileName(filename), theDataBase(0),
  theHasTableNames ( false ), theRow(0), theBlockCtr(0)
{
  if ( theFileName.size() )
    theDataBase = new DataBase ( theFileName.c_str() );
}

string SqliteReader::filename() const
{
  return theFileName;
}

SqliteReader * SqliteReader::clone ( const string & file ) const
{
  return new SqliteReader ( file );
}

int SqliteReader::getNumberOfEntries() const
{
  return -1; // -1 means you dont know
}

void SqliteReader::reset()
{
  // reset
}

void SqliteReader::getTableNames()
{
  theDataBase->exe ( "select name from sqlite_master where type IN ('table','view') ;" );
  for ( vector< string >::const_iterator i=theDataBase->vdata.begin(); 
        i!=theDataBase->vdata.end() ; ++i )
  {
    theTableNames.push_back ( *i );
  }
  theHasTableNames=true;
  theTableIter=theTableNames.begin();
}

void SqliteReader::nextBlock()
{
  // read the next few rows
  ostringstream cmd;
  theDataBase->vcol_head.clear();
  theDataBase->vdata.clear();

  cmd << "select * from " << *(theTableIter) << " limit " << theMaxRows
      << " offset " << theMaxRows*theBlockCtr << ";";
  // cout << "[SqliteReader] cmd=" << cmd.str() << endl;
  int rc=theDataBase->exe ( cmd.str() );
  if ( rc!=0 ) cout << "[SqliteReader] rc=" << rc << endl;
  int ncols = theDataBase->vcol_head.size();
  if ( ncols )
  {
    theRowsInBlock = theDataBase->vdata.size() / ncols;
  }
}

Tuple SqliteReader::next ()
{
  // cout << "[SqliteReader] row=" << theRow << " block=" << theBlockCtr << endl;
  if (!theHasTableNames ) getTableNames();
  // cout << "[SqliteReader] next tuple!" << endl;
  if ( theTableIter==theTableNames.end() )
  {
    throw TerminateException();
  }

  if ( theRow==0 ) nextBlock();

  // we have ncols columns
  unsigned ncols = theDataBase->vcol_head.size();

  if ( ncols == 0 )
  {
    // continue with next table
    theBlockCtr=0;
    theRow=0;
    theTableIter++;
    return next();
  }

  Tuple t ( *(theTableIter) );

  int c=0;
  for ( vector< string >::const_iterator i=theDataBase->vcol_head.begin(); 
        i!=theDataBase->vcol_head.end() ; ++i )
  {
    if ( (*i) == "name" ) continue;
    // cout << *i << endl;
    MultiType mt = theDataBase->vdata[ ncols*theRow + c];
    mt.trimType();
    t[*i]=mt;
    c++;
  }

  theRow++;
  if ( theRow == theRowsInBlock )
  {
    theRow=0;
    theBlockCtr++;
  }
  t.fill();
  return postProcess ( t );
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<SqliteReader> t ( "sqlite" );
}

