#include <dataharvester/HarvestingConfiguration.h>
#include "HBookReader.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/SimpleConfigurable.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <THbookFile.h>
#include <TList.h>
#include <TSystem.h>
#include <TTree.h>
#include <THbookTree.h>
#include <TLeaf.h>
#include <THbookKey.h>

using namespace std;
using namespace dataharvester;

namespace {
  bool useTitles()
  {
    bool ret = SimpleConfigurable < bool >
              ( false, "HBookReader:UseTitlesAsNames" ).value();
    return ret;
  }

  bool storeTitles()
  {
    bool ret = SimpleConfigurable < bool >
              ( false, "HBookReader:StoreTitles" ).value();
    return ret;
  }
}

HBookReader::HBookReader( const string & filename ) : theFileName ( filename ),
    theFile (0), theList(0), theCurTree(0),
    theTupleCounter(0), theKeyCounter ( 0 )
{
  if ( filename == "" ) return;
  gSystem->Load("libHbook");
  theFile = new THbookFile ( theFileName.c_str() );
  theList = theFile->GetListOfKeys();
  getNewKey();
};

HBookReader::HBookReader ( THbookFile * f ) : theFileName ( f->GetName() ),
  theFile ( f ), theList(0), theCurTree(0),
  theTupleCounter(0), theKeyCounter(0)
{
  if ( theFileName == "" ) return;
  theList = theFile->GetListOfKeys();
  getNewKey();
}

HBookReader::~HBookReader()
{
  if ( theList ) delete theList;
  if ( theFile )
  {
    theFile->Close();
    delete theFile;
  };
};

void HBookReader::reset()
{
  cout << "[HBookReader] reset not implemented" << endl;
}

int HBookReader::getNumberOfEntries() const
{
  int tupleCouterBackup=theTupleCounter;
  int keyCounterBackup=theKeyCounter;
  TTree * curTreeBackup = theCurTree;
  int ctr=0;
  while ( true )
  {
    if (!theCurTree)
    {
      theCurTree = curTreeBackup;
      theKeyCounter = keyCounterBackup;
      theTupleCounter = tupleCouterBackup;
      return ctr;
    };
    ctr+=(int) theCurTree->GetEntries();
    bool ret = getNewKey();
    if (!ret)
    {
      theCurTree = curTreeBackup;
      theKeyCounter = keyCounterBackup;
      theTupleCounter = tupleCouterBackup;
      return ctr;
    };
  };
  cout << "[HBookReader] huh? " << __LINE__ << endl;
  return ctr;
};

HBookReader * HBookReader::clone ( const string & file ) const
{
  return new HBookReader ( file );
};

string HBookReader::filename() const
{
  return theFileName;
}

bool HBookReader::nextNtuple ( string & ntuplename,
                                  map < string, MultiType > & usermap )
{

  if (!theCurTree) return false;
  if ( theTupleCounter >= theCurTree->GetEntries() )
  {
    bool ret = getNewKey(); // next key
    if (!ret) return false;
  };

  // FIXME ignore meta data if this is desired
  ntuplename=theCurTree->GetName();

  if ( useTitles() )
  {
    cout << "[HBookReader] use titles." << endl;
    string title="";
    if ( theCurHTree )
    {
      title= theCurTree->GetTitle();
      // title = (theFile->ConvertRWN( theCurHTree->GetID() )->GetTitle();
      cout << "title=" << title << endl;
    } else {
      cout << "[HBookReader] no curhtree!" << endl;
    }
    if ( !(title.empty() ))
    {
      ntuplename=title;
    }
  }

  while ( Helpers::ignoreMetaData() &&
        ( ntuplename == "__content__" || ntuplename == "__ntuples__" ) )
  {
    return nextNtuple ( ntuplename, usermap );
    /*
    bool ret = getNewKey();
    if (!ret) return false;
    ntuplename=theCurTree->GetName();
    */
  };

  theCurTree->GetEntry( theTupleCounter );
  theTupleCounter++;
  usermap.clear();

  /*
  if ( storeTitles() && (!(title.empty() ) ))
  {
    usermap["title"]=title;
  }*/

  // get branches
  int n_branches = theCurTree->GetListOfBranches()->GetEntries();
  for ( int i=0; i< n_branches ; i++ )
  {
    string branchname = theCurTree->GetListOfBranches()->At(i)->GetName();
    const TLeaf * leaf = theCurTree->GetLeaf(branchname.c_str());
    if ( !leaf )
    {
      cout << "[HBookReader] error: leaf \"" << branchname << "\" not available!" << endl;
      break;
    }
    string type =  leaf->GetTypeName();
    if ( type.find("edm::Wrapper") != string::npos )
    {
      cout << "[HBookReader] Warning: skipping edm::Wrapper object: " << type << endl;
      continue;
    }
    void * vp = leaf->GetValuePointer();
    usermap[branchname]=leaf->GetValue();

    if ( type == "Char_t" )
    {
      // must be a string
      string value = (char *) (vp);
      usermap[branchname]=value;
    } else if ( type == "Int_t" ) {
      // make sure an int remains an int
      long value = atol ( usermap[branchname].asString().c_str() );
      // cout << endl << "[RDS debug] int!" << value << endl;
      usermap[branchname]=value;
    } else if ( type == "Double_t" ) {
      // make sure a double remains a double
      double value = atof ( usermap[branchname].asString().c_str() );
      // cout << "[HBookReader] " << branchname << " is double" << endl;
      usermap[branchname]=value;
    };

    // usermap[branchname].trimType();
  };
  return true;
};

THbookFile * HBookReader::file()
{
  return theFile;
}

bool HBookReader::getNewKey() const
{
  if ( theKeyCounter > theList->LastIndex() ) return false;
  THbookKey * key = (THbookKey *) theList->At( theKeyCounter );
  if ( !key ) return false;
  string name = key->GetName();
  name=name.substr(1,name.size()-1);
  int n = atoi ( name.c_str() );
  //TObject * o =  theFile->FindObjet ( name );
  TObject * o =  theFile->Get ( n );
  if (!o) return false;
  if ( (string) o->ClassName() == (string) "THbookTree" )
  {
    theCurHTree = (THbookTree *) o;
    theCurTree = (TTree *) theFile->ConvertRWN ( n );
    // theCurHTree = (THbookTree *) theFile->ConvertRWN ( n );
  } else {
    cout << "[HBookReader] skipping " << name << ": is a " << o->ClassName() << endl;
    theKeyCounter++;
    return getNewKey();
  }
  theTupleCounter=0;
  theKeyCounter++;
  return true;
};

Tuple HBookReader::next ( )
{
  string tuplename="???";
  map < string, MultiType > data;
  bool ret = nextNtuple ( tuplename, data );
  if ( ret )
  {
    Tuple t ( tuplename );
    for ( map < string, MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
    {
      t[i->first]=i->second; // .asString();
    };

    return postProcess ( t );
  } else {
    throw TerminateException ();
  };
};


#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<HBookReader> t( "hbook" );
};

