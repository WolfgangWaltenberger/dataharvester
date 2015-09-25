#include <dataharvester/HarvestingConfiguration.h>
#include "GzipWriter.h"
// #include <dataharvester/XmlWriter.h>
#include <dataharvester/WriterManager.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/Mode.h>
#include <fstream>

using namespace dataharvester;
using namespace std;

void GzipWriter::save ( const Tuple & d, const string & prefix )
{
  // Tuple tmp ( d );
  // const_cast <Tuple &> (tmp).fill ( "", false );
  theStreamer->save ( d, prefix );
  // writeStream();
}

void GzipWriter::getTargetFormat()
{
  string format = theFileName;
  format.replace ( format.size()-3,3,"" ); // remove .gz$
  string::size_type pos = format.rfind(".");
  if ( pos != string::npos && pos >= 0 )
  {
    theTargetFormat = format.substr ( pos+1 );
  }
}

void GzipWriter::instantiateStreamer()
{
  getTargetFormat();
  theStreamer = WriterManager::self()->streamableWriter ( theTargetFormat );
  /*
  if ( theTargetFormat=="xml" )
  {
    theStreamer = new XmlWriter("");
  } else if ( theTargetFormat=="tnt" ) {
    theStreamer = new TntWriter("");
  } else {
    theStreamer = new TextWriter("");
  }
  */
  theStreamer->setStream ( theStream, false );
  /* if ( theTargetFormat=="xml" )
    dynamic_cast <XmlWriter *> (theStreamer)->printHeader();
    */
}

void GzipWriter::writeStream()
{
  string str = theStream.str();

  if ( theFile )
  {
    unsigned int sz = gzputs ( theFile, str.c_str() );
    if ( sz != str.size() )
    {
      cout << "[GzipWriter] write error: " << sz << " instead of " << str.size()
           << " characters written!" << endl;
    };
  };
  theStream.clear();
}

GzipWriter::~GzipWriter()
{
  if ( theStreamer ) delete theStreamer;
  writeStream();
  gzclose ( theFile );
}

GzipWriter::GzipWriter ( const string & filename ) :
  theStreamer ( 0 ), theFileName ( filename ), theFile(0),
  theTargetFormat("txt")
{
  if ( theFileName == "" ) return;
  string mode="wb";
  if ( Mode::fileMode() == Mode::Update )
  {
    mode="wb";
    cout << "[GzipWriter] Error: trying to append to gzip file. This is currently not possible"
         << endl;
  }
  theFile = gzopen ( theFileName.c_str() , mode.c_str() );
  if ( theFile == 0 )
  {
    throw HarvestingException ( (string) "could not open " + theFileName );
  };
  instantiateStreamer();
}

GzipWriter * GzipWriter::clone( const string & filename ) const
{
  return new GzipWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder<GzipWriter> t( "gz" );
}

