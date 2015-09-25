#include <dataharvester/HarvestingConfiguration.h>
#include "GzipReader.h"
#include <dataharvester/ReaderManager.h>
#include <dataharvester/StreamableReader.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/TerminateException.h>
#include <fstream>

using namespace dataharvester;
using namespace std;

namespace {
  int countNewlines( char * buf, int buflen )
  {
    int ret=0;
    for ( int i=0; i< buflen ; ++i ) 
    {
      if ( buf[i] == '\n' )
      {
        ret++;
        buf[i]='x';
      };
    };
    return ret;
  }
}

void GzipReader::openFile()
{
  if ( theFileName == "" ) return;
  theFile = gzopen ( theFileName.c_str(), "rb" );
  if ( theFile == NULL )
  {
    throw HarvestingException ( "gzopen error" );
  };
}

int GzipReader::getNumberOfEntries() const
{
  gzFile f = gzopen ( theFileName.c_str(), "r" );
  if (f == Z_NULL ) return 0;

  static const int buflen=1048576;
  char buf[buflen];
  int ret = 0;

  int cc = 1;
  while ( cc )
  {
    cc = gzread ( f, buf, buflen );
    ret +=countNewlines ( buf, buflen );
    if ( cc == -1 )
    {
      cout << "[GzipReader] Error when counting number of tuple rows!" << endl;
      break;
    };
  };
  gzclose ( f );
  return ret;
}

GzipReader::~GzipReader()
{
  cout << "[GzipReader] closing" << endl;
  gzclose ( theFile );
}

GzipReader::GzipReader( const string & filename ) : AbstractReader(),
  theFileName ( filename ), theBuffer ("")
{
  openFile();
}

GzipReader * GzipReader::clone ( const string & file ) const
{
  return new GzipReader ( file );
}

string GzipReader::filename() const
{
  return theFileName;
}

void GzipReader::reset()
{
  gzrewind ( theFile );
  // cout << "[GzipReader] reset not implemented" << endl;
}

string GzipReader::readLine()
{
  char tmp[theMaxLen];
  char * ret = gzgets ( theFile, tmp, theMaxLen );
  if ( !ret )
  {
    throw HarvestingException();
  };
  string s ( tmp );
  if ( s[s.size()-1]=='\n' )
  {
    s.replace ( s.size()-1,1,"" );
  };
  return s;
}

Tuple GzipReader::next ( )
{
  StreamableReader * seeder = ReaderManager::self()->streamableReader ( "txt" );
  seeder->clear();
  string line="";
  try {
    do {
      line = readLine();
    } while ( !(seeder->addLine ( line )) );
    Tuple ret = seeder->getCurrentTuple();
    return postProcess ( ret );
  } catch ( ... ) {
    throw TerminateException();
  }
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<GzipReader> gzip ( "gz" );
}

