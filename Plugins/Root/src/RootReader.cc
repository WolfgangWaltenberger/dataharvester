#include <dataharvester/HarvestingConfiguration.h>
#include "RootReader.h"
#include "RootHelpers.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/SimpleConfigurable.h>
#include <fstream>
#include <sstream>
#include <TFile.h>
#include <TList.h>
#include <TTree.h>
#include <TLeaf.h>
#include <TSystem.h>
#include <TKey.h>
#include <TVirtualIndex.h>

using namespace std;
using namespace dataharvester;

namespace {
  bool useTitles()
  {
    bool ret = SimpleConfigurable < bool >
              ( false, "RootReader:UseTitlesAsNames" ).value();
    return ret;
  }

  bool storeTitles()
  {
    bool ret = SimpleConfigurable < bool >
              ( false, "RootReader:StoreTitles" ).value();
    return ret;
  }

  void removeDot ( string & name )
  {
    if ( name.size() == 0 ) return;
    if ( name[name.size()-1]=='.' ) name=name.substr(0,name.size()-1) ;
  }

}
  
void RootReader::addEntries ( Tuple & t, TBranch * branch, const std::string & prefix ) const
{
  string sprefix=prefix;
  if ( prefix!="" ) sprefix+=":";
  string branchname = RootHelpers::getShortBranchname ( branch, prefix );
  removeDot ( branchname );

  vector < map < string, MultiType > > m = 
    RootHelpers::getNested ( branch, prefix, theOffsets[prefix] );
  theOffsets[prefix]+=m.size();

  string nextname=sprefix+branchname;
  for ( vector< map < string, MultiType > >::const_iterator row=m.begin(); row!=m.end() ; ++row )
  {
    for ( map < string, MultiType >::const_iterator data=row->begin(); data!=row->end() ; ++data )
    {
      if ( t.hasValue((nextname+":"+data->first).c_str() ))
      {
        cout << "[RootReader] error, trying to overwrite " << nextname+":"+data->first
             << endl;
      }
      // if (!t.hasValue((nextname+":"+data->first).c_str() ))
        t[nextname+":"+data->first]=data->second;
    }
    t.fill ( nextname, false );
  }

  vector < MultiType > leaves = 
    RootHelpers::getValues ( branch, theOffsets[branch->GetName()] );
  theOffsets[branch->GetName()]++;
  for ( vector< MultiType >::const_iterator lf=leaves.begin(); lf!=leaves.end() ; ++lf )
  {
    t[sprefix+branchname]=*lf;
    if ( prefix.size() ) t.fill ( prefix, false );
  }
}

void RootReader::addBranch ( Tuple & t, TBranch * branch, const string & prefix, int evt )
{
  string sprefix=prefix;
  if ( prefix!="" ) sprefix+=":";

  int nsubbrs = branch->GetListOfBranches()->GetEntries();

  if ( nsubbrs > 0 )
  {
    for ( int sub=0; sub< nsubbrs  ; sub++ )
    {
      TBranch * bsub = (TBranch * ) (branch->GetListOfBranches()->At(sub));
      bsub->SetAutoDelete ( kTRUE );
      int bsubsubnrs=bsub->GetListOfBranches()->GetEntries();
      string name = branch->GetName();
      removeDot ( name );
      if ( bsubsubnrs > 0 ) addBranch ( t, bsub, name, evt );
    }
  }

  string branchname = RootHelpers::getShortBranchname ( branch, prefix );

  removeDot ( branchname );

  vector < map < string, MultiType > > m = 
    RootHelpers::getNested ( branch, prefix, theOffsets[prefix] );
  theOffsets[prefix]+=m.size();

  string nextname=sprefix+branchname;

  for ( vector< map < string, MultiType > >::const_iterator row=m.begin(); 
        row!=m.end() ; ++row )
  {
    for ( map < string, MultiType >::const_iterator data=row->begin(); 
          data!=row->end() ; ++data )
    {
      t[nextname+":"+data->first]=data->second;
    }
    t.fill ( nextname, false );
  }

  vector < MultiType > leaves = 
    RootHelpers::getValues ( branch, theOffsets[branch->GetName()] );
  theOffsets[branch->GetName()]++;
  for ( vector< MultiType >::const_iterator lf=leaves.begin(); lf!=leaves.end() ; ++lf )
  {
    t[nextname]=*lf;
    if ( prefix.size() ) t.fill ( prefix, false );
  }
}

RootReader::RootReader( const string & filename ) : AbstractReader (), 
    theFileName ( filename ), theFile (0), theList(0), 
    theTupleCounter(0), theKeyCounter ( 0 ), theSubDirKeyCounter ( 0 ),
    theEventCounter(0)
{
  static bool once=false;
  if ( once && filename.size() )
  {
    cout << "[RootReader] WARNING: this reader is still highly experimental!" << endl;
    once=false;
  }
  RootHelpers::setCurTree ( 0 );
  RootHelpers::ignoreWarnings();
  if ( filename == "" ) return;
  theFile = TFile::Open ( theFileName.c_str() );
  theList = (TList *) theFile->GetListOfKeys(); // ->Clone();
  getNewKey();
}

RootReader::RootReader( const RootReader & o ) : AbstractReader ( o ), 
    theFileName ( o.theFileName ), theFile (0), theList(0), 
    theTupleCounter(0), theKeyCounter ( 0 ), theSubDirKeyCounter ( 0 ),
    theEventCounter(0)
{
  RootHelpers::setCurTree ( 0 );
  if ( theFileName == "" ) return;
  theFile = new TFile ( theFileName.c_str() );
  theList = theFile->GetListOfKeys();
  getNewKey();
}

RootReader::RootReader ( TFile * f ) : AbstractReader(), 
  theFileName ( f->GetName() ), theFile ( f ), theList(0), 
  theTupleCounter(0), theKeyCounter(0), theSubDirKeyCounter ( 0 ),
  theEventCounter(0)
{
  RootHelpers::setCurTree ( 0 );
  RootHelpers::ignoreWarnings();
  if ( theFileName == "" ) return;
  theList = theFile->GetListOfKeys();
  getNewKey();
}

RootReader::~RootReader()
{
  if ( theList ) delete theList;
  if ( theFile )
  {
    theFile->Close();
    delete theFile;
  };
}

void RootReader::reset()
{
  theEventCounter=0;
  cout << "[RootReader] reset not implemented" << endl;
}

int RootReader::getNumberOfEntries() const
{
  int tupleCouterBackup=theTupleCounter;
  int keyCounterBackup=theKeyCounter;
  int subkeyCounterBackup=theSubDirKeyCounter;
  TTree * curTreeBackup = RootHelpers::curTree();
  int ctr=0;
  while ( true )
  {
    if (!RootHelpers::curTree())
    {
      RootHelpers::setCurTree ( curTreeBackup );
      theKeyCounter = keyCounterBackup;
      theSubDirKeyCounter = subkeyCounterBackup;
      theTupleCounter = tupleCouterBackup;
      return ctr;
    };
    ctr+=(int) RootHelpers::curTree()->GetEntries();
    bool ret = getNewKey();
    if (!ret)
    {
      RootHelpers::setCurTree ( curTreeBackup );
      theKeyCounter = keyCounterBackup;
      theSubDirKeyCounter = subkeyCounterBackup;
      theTupleCounter = tupleCouterBackup;
      return ctr;
    };
  };
  cout << "[RootReader] huh? " << __LINE__ << endl;
  return ctr;
}

RootReader * RootReader::clone ( const string & file ) const
{
  return new RootReader ( file );
}

string RootReader::filename() const
{
  return theFileName;
}

TFile * RootReader::file()
{
  return theFile;
}

bool RootReader::getNewKey() const
{
  RootHelpers::setCurTree ( 0 );
  if ( theKeyCounter > theList->LastIndex() ) return false;
  TKey * key = (TKey *) theList->At( theKeyCounter );
  if ( !key ) return false;
  ostringstream snme;
  snme << key->GetName() << ";" << key->GetCycle();
  string name = snme.str();
  // cout << "[RootReader] new key: " << theKeyCounter << ":" << name << endl;
  TObject * o =  theFile->Get ( name.c_str() );
  if (!o) return false;
  if ( (string) o->ClassName() == (string) "TTree" )
  {
    RootHelpers::setCurTree ( (TTree *) o );
  } else if ( (string) o->ClassName() == (string) "TDirectoryFile" ) {
    string dirname=name;
    if ( dirname.find(";")!=string::npos )
    {
      dirname=dirname.substr(0,dirname.find(";") );
    }
    /*
    cout << "[RootReader] ok, we are in subdir " << dirname << 
         " at " << theSubDirKeyCounter << endl;
         */
    TDirectoryFile * tf =  (TDirectoryFile *) (o);
    // cout << "[RootReader] " << (void *) tf << ", last index is " 
         //<< tf->GetListOfKeys()->LastIndex() << endl;
    if ( theSubDirKeyCounter > tf->GetListOfKeys()->LastIndex() )
    {
      /* cout << "[RootReader] ok, seems like subdir " << tf->GetName() 
           << " is done." << endl; */
      theSubDirKeyCounter=0;
      theKeyCounter++;
      return getNewKey();
    }
    TString tuplename=tf->GetListOfKeys()->At(theSubDirKeyCounter)->GetName();
    // cout << "[RootReader] at " << theSubDirKeyCounter << " we have " << tuplename << endl;
    TObject * t = theFile->Get( (dirname+"/"+tuplename) );
    RootHelpers::setCurTree ( (TTree *) t );
    theSubDirKeyCounter++;
    theTupleCounter=0;
    return true;
    // theKeyCounter++;
    // getNewKey();
  } else {
    cout << "[RootReader] skipping " << name << ": is a " << o->ClassName() << endl;
    theKeyCounter++;
    return getNewKey();
  }
  theTupleCounter=0;
  // theEventCounter=0;
  theKeyCounter++;
  return true;
}

Tuple RootReader::next()
{
  if (!RootHelpers::curTree() ) throw TerminateException( "no current tree");
  if ( theTupleCounter >= RootHelpers::curTree()->GetEntries() )
  {
    bool ret = getNewKey(); // next key
    if (!ret) throw TerminateException();
  };

  string ntuplename=RootHelpers::curTree()->GetName();
  if ( RootHelpers::curTree()->GetDirectory() )
  {
    string dirname=RootHelpers::curTree()->GetDirectory()->GetName();
    if (false)
    {
      ntuplename=dirname+"_"+ntuplename;
    }
  }

  string title = RootHelpers::curTree()->GetTitle();
  if ( useTitles() && (!(title.empty() ) ))
  {
    ntuplename=title;
  }
  
  Tuple t(ntuplename);

  if ( storeTitles() && (!(title.empty() ) ))
  {
    t["title"]=title;
  }
  
  addCurrentTree ( t );

  // t.dump();
  theTupleCounter++;
  t.fill("",false);

  return postProcess ( t );
}

void RootReader::addIndexedTree ( Tuple & t )
{
  // RootHelpers::curTree() has an Index!!
  TVirtualIndex * indx=RootHelpers::curTree()->GetTreeIndex();
  int n = indx->GetN();
  if ( theEventCounter >= n )
  {
    cout << "[RootReader]  Huh? theEventCounter=" << theEventCounter 
         << ", n=" << n << "?!?" << endl;
    // throw TerminateException( "enough events collected" );
  }

  RootHelpers::curTree()->ResetBranchAddresses();
  RootHelpers::curTree()->GetEntry( theEventCounter, 1 );
  // RootHelpers::curTree()->LoadTree ( theEventCounter );

  int n_branches = RootHelpers::curTree()->GetListOfBranches()->GetEntries();
  // cout << "[RootReader] adding branches" << endl;
  for ( int i=0; i< n_branches ; i++ )
  {
    // char mem[65536];
    TBranch * curbranch = (TBranch *) (RootHelpers::curTree()->GetListOfBranches()->At(i));
    addBranch ( t, curbranch, "", theEventCounter );
    // curbranch->SetAddress ( 0 );
    curbranch->SetAutoDelete ( kTRUE );
    // RootHelpers::curTree()->Refresh();
  };
  // cout << "[RootReader] /" << endl;
  theEventCounter++;
}

void RootReader::addCurrentTree ( Tuple & t )
{
  TVirtualIndex * indx=RootHelpers::curTree()->GetTreeIndex();
  
  if ( indx )
  {
    addIndexedTree ( t );
    return;
  }

  RootHelpers::curTree()->ResetBranchAddresses();
  RootHelpers::curTree()->GetEntry ( theTupleCounter, 1 );
  int n_branches = RootHelpers::curTree()->GetListOfBranches()->GetEntries();
  for ( int i=0; i< n_branches ; i++ )
  {
    TBranch * curbranch = (TBranch *) (RootHelpers::curTree()->GetListOfBranches()->At(i));
    addEntries ( t, curbranch );
    // cout << "[RootReader] adding " << curbranch->GetName() << " to " << t.getName() << endl;
  };
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<RootReader> t( "root" );
}

