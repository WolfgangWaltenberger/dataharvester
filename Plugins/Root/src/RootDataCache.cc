#include <dataharvester/HarvestingConfiguration.h>
#include "RootDataCache.h"
#include <dataharvester/Mode.h>
#include <dataharvester/Helpers.h>
#include <TFile.h>
#include <sstream>

using namespace std;
using namespace dataharvester;

bool RootDataCache::needsSave() const
{
  return theNeedsSave;
}

void RootDataCache::setNeedsSave ( )
{
  theNeedsSave=true;
}

RootDataCache::RootDataCache() : theNeedsSave ( false ),
    theFileName ( "unknown.root" ), theFile ( 0 )
{}

void RootDataCache::setFileName ( string f )
{
  theFileName = f;
}

RootDataCache::RootDataCache ( const RootDataCache & o ) :
  theNeedsSave ( o.theNeedsSave ),
  theFileName ( o.theFileName ), theFile ( 0 )

{}

RootDataCache::~RootDataCache()
{
  // write();
  if ( theFile ) delete theFile;
}

void RootDataCache::add ( string ntple, const map < string, MultiType > & source,
   string description )
{
  map < string, MultiType > mp = source;

  for ( vector< RootInternalNtuple >::iterator i=theData.begin(); 
        i!=theData.end() ; ++i )
  {
    if ( i->name() == ntple )
    {
      i->add ( mp );
      return;
    };
  };
  /*
  bool isfirst=false;
  if ( theHasFirstWritten[ntple]==false )
  {
    theHasFirstWritten[ntple]=false; // false; // FIXME true
    isfirst=true;
  }*/
  RootInternalNtuple newtuple ( ntple, mp /*, isfirst */ );
  theData.push_back ( newtuple );
  bool describe_ntuples = true;
  if ( Mode().fileMode() == Mode::Update )
  {
    describe_ntuples = false;
  };

  if ( describe_ntuples && ntple != "__content__" && description != "" )
  {
    map < string, MultiType > data;
    data["name"]=ntple;
    data["description"]=description;
    ostringstream ostr;
    for ( map< string, MultiType >::const_iterator nt=mp.begin(); 
          nt!=mp.end() ; ++nt )
    {
      if ( nt != mp.begin() ) ostr << ":";
      ostr << nt->first;
    };
    data["format"]=ostr.str();
    add ( "__content__", data, "Description of all ntuples in the file." );
  };
}

void RootDataCache::openFile()
{
  if ( theFile == 0 )
  {
    theFile = new TFile ( theFileName.c_str(), 
        Mode::fileModeString().c_str() );
    if ( Helpers::verbose() )
    {
      cout << "[RootDataCache] Create file " << theFileName
           << " with \"" << Mode::fileModeString() << "\"" << endl;
    };
  };

}

void RootDataCache::write()
{
  if ( theFileName == "" ) return;
  if ( !needsSave() ) return;
  Helpers::createBackup ( theFileName );
  openFile();

  if ( Helpers::verbose() )
  {
    cout << "[RootDataCache] Write to file " << theFile->GetName() << endl;
  };

  theFile->cd();

  for ( vector< RootInternalNtuple >::iterator ntpl=theData.begin();
        ntpl!=theData.end() ; ++ntpl )
  {
    ntpl->write();
  };
  theData.clear();
  theNeedsSave = false;
}

void RootDataCache::describe() const
{
  cout << "[RootDataCache] we have " << theData.size() << " tuples: " << endl;
  for ( vector< RootInternalNtuple >::const_iterator i=theData.begin(); i!=theData.end() ; ++i )
  {
    i->describe();
  }
}

int RootDataCache::nTuples() const
{
  return theData.size();
}

void RootDataCache::close()
{
  write();
  if ( theFile )
  {
    theFile->Close();
    delete theFile;
    theFile=0;
  }
}

