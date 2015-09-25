#include <dataharvester/HarvestingConfiguration.h>
#include "RootInternalNtuple.h"
#include <dataharvester/Helpers.h>
#include <TTree.h>
#include "TROOT.h"
#include "TSystem.h"
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace {
  void loadVector ()
  {
    static bool hasLoaded=false;
    //cout << "[RootInternalNtuple] trying to load vector " << hasLoaded << endl;
    if (hasLoaded) return;
    gSystem->Load("libCintex");
    gROOT->ProcessLine("#include <vector>");
    hasLoaded=true;
    //cout << "[RootInternalNtuple] /trying to load vector" << endl;
  }
}

RootInternalNtuple::RootInternalNtuple (
    const string & name, const map < string, MultiType > & mp,
    bool firstwrite ) :
    theName ( name ), firstWrite(firstwrite), theHeader ( mp ), theTree (0)
{
  theChecker.define ( mp );
  add ( mp );
}

RootInternalNtuple::~RootInternalNtuple ()
{
  if ( theTree )
  {
    theTree->Write();
    delete theTree;
    theTree=0;
  };
  for ( map< string, char * >::iterator i=theStrings.begin();
        i!=theStrings.end() ; ++i )
  {
    free ( i->second );
  };
  theStrings.clear();
  theStringsSize.clear();

  for ( map< string, vector < long int > * >::iterator i=theIntVecs.begin();
        i!=theIntVecs.end() ; ++i )
  {
    delete ( i->second );
  };

  theIntVecs.clear();
  for ( map< string, vector < double > * >::iterator i=theDoubleVecs.begin();
        i!=theDoubleVecs.end() ; ++i )
  {
    delete ( i->second );
  };

  theDoubleVecs.clear();
}

void RootInternalNtuple::add ( const map < string, MultiType > & mp )
{
  bool chk = theChecker.check ( mp );
  if (!chk) return;
  vector < MultiType > newData;
  int ctr=0;
  for ( map< string, MultiType >::const_iterator i=mp.begin(); 
        i!=mp.end() ; ++i )
  {
    newData.push_back ( i->second );
    ctr++;
  };
  theData.push_back ( newData );
}

string RootInternalNtuple::name() const { return theName; }


void RootInternalNtuple::initWrite()
{
  /**
   *  initWrite prepares theDoubles, theInts, theStrings, theStringsSize,
   *  theIntVecs, theDoubleVecs
   *  for the TTree. Also sets the TTrees branch addresses.
   */
  if ( !firstWrite ) return;
  if ( Helpers::verbose() > 2 )
  {
    cout << "[RootInternalNtuple::initWrite] create TTree " << theName << endl;
  };

  theTree = new TTree ( theName.c_str(), theName.c_str() );
  map < string, MultiType > branches = theHeader;

  for ( map < string, MultiType >::const_iterator
        i=branches.begin(); i!=branches.end() ; ++i )
  {
    ostringstream descr;
    string branchname = i->first;

    if ( branchname.size() > 63 )
    {
      cout << "[RootInternalNtuple] warning: branchname exceeds 64 characters."
           << " Root seems to not allow this. Taking first 64 characters." << endl;
      branchname=branchname.substr(0,63);
    }
    // FIXME: checking if this resolves the issue of equally named
    //    branches in different trees
    // according to root 4.08 manual chap12 p.218 
    //    the "<branchname>." is explicit notation for this case
    descr << branchname; // << ".";

    if ( Helpers::verbose() & 8 )
    {
      cout << "[RootInternalNtuple]     `-- Create Branch " << branchname << endl;
    }

    if ( i->second.isA() == "float" || i->second.isA() == "double" )
    {
      descr << "/D";
      theDoubles[branchname]=(double) i->second;
      theTree->Branch ( branchname.c_str(), 
          (void *) &(theDoubles[branchname]),
          descr.str().c_str() );
    } 
    else if ( i->second.isA() == "int32" || i->second.isA() == "int64" || i->second.isA() == "bool" ) 
    {
      descr << "/I";
      theInts[branchname]=(int) i->second;
      theTree->Branch ( branchname.c_str(), 
          (void *) &(theInts[branchname]),
          descr.str().c_str() );
    } else if ( i->second.isA() == "string" ) {
      descr << "/C";
      // determine string length 
      unsigned int sze=Helpers::maxStringLength();
      /*
      if ( i->second.asString().size() > sze )
      {
        sze=i->second.asString().size()+1;
      };*/
      // oh no! malloc!! hell is loose!
      theStrings[branchname]=(char *) malloc ( sze+1 );
      theStringsSize[branchname]=sze;
      snprintf ( theStrings[branchname], sze, "%s",
                 ((string) i->second).c_str() );
      theTree->Branch ( branchname.c_str(), theStrings[branchname],
                        descr.str().c_str() );

    } else if ( i->second.isA() == "doublevec" || i->second.isA() == "floatvec" ) {
      loadVector();
      theDoubleVecs[branchname]=new vector < double > ();
      theTree->Branch ( branchname.c_str(), "vector<double>",
          &(theDoubleVecs[branchname]) );
    } else if ( i->second.isA() == "int32vec" || i->second.isA() == "int64vec" ) {
      loadVector();
      theIntVecs[branchname]=new vector < long int > ();
      theTree->Branch ( branchname.c_str(), "vector<long>",
          &(theIntVecs[branchname]) );
    } else {
      cout << "[RootInternalNtuple] ntuple " << branchname
           << " consists of " << i->second.isA()
           << ". Dont know how to handle them." << endl;
    };

    if ( Helpers::verbose() > 8 )
    {
    // FIXME: DEBUG...
    //    the verbose() dont work for me (Gerald)
      cout << "[RootInternalNtuple]     -- Branch Creation:" << endl
        << "\tfound type: " << i->second.isA() << endl
        << "\tname: " << branchname << endl
        << "\tdescriptor: " << descr.str()
        << endl;
    }
  };
  firstWrite=false;
}

void RootInternalNtuple::write()
{
  if ( theData.size() == 0 )
  {
    if ( Helpers::verbose() )
    {
      cout << "[RootInternalNtuple] empty ntuple." << endl;
    };
    return;
  };

  initWrite();
  // describe();

  for ( vector < vector < MultiType > >::iterator pvrow=theData.begin();
        pvrow!=theData.end() ; ++pvrow )
  {
    // recreate original map
    vector < MultiType > & vrow = *pvrow;
    if ( vrow.size() != theHeader.size() )
    {
      cout << "[RootInternalNtuple] Error: ntuple size changed!!!" << endl;
      cout << "[RootInternalNtuple] Ignoring data!!" << endl;
      return;
    };
    map < string, MultiType > row;
    int ctr = 0;
    for ( map < string, MultiType >::iterator i=theHeader.begin();
          i!=theHeader.end() ; ++i )
    {
      string branchname= i->first;
      row[branchname]=vrow[ctr];
      ctr++;
    };

    for ( map < string, double >::iterator f=theDoubles.begin();
          f!=theDoubles.end() ; ++f )
    {
      string branchname = f->first;
      f->second = (double) row[branchname];
      if ( Helpers::verbose() > 16 )
      {
        cout << "[RootInternalNtuple] ("
             << theName << ") " << branchname << "=" << f->second << endl;
      };
    };

    for ( map < string, vector < double > * >::iterator f=theDoubleVecs.begin();
          f!=theDoubleVecs.end() ; ++f )
    {
      f->second->clear();
      string branchname = f->first;
      vector < double > values = row[branchname].asDoubleVector();
      for ( vector< double >::const_iterator i=values.begin(); i!=values.end() ; ++i )
      {
        f->second->push_back ( *i );
      }
      if ( Helpers::verbose() > 16 )
      {
        cout << "[RootInternalNtuple] ("
             << theName << ") " << branchname << "=" << f->second << endl;
      };
    };
    for ( map < string, vector < long int > * >::iterator f=theIntVecs.begin();
          f!=theIntVecs.end() ; ++f )
    {
      f->second->clear();
      string branchname = f->first;
      vector < long int > values = row[branchname].asIntVector();
      for ( vector< long int >::const_iterator i=values.begin(); i!=values.end() ; ++i )
      {
        f->second->push_back ( *i );
      }
      if ( Helpers::verbose() > 16 )
      {
        cout << "[RootInternalNtuple] ("
             << theName << ") " << branchname << "=" << f->second << endl;
      };
    };
    for ( map < string, int >::iterator f=theInts.begin();
          f!=theInts.end() ; ++f )
    {
      string branchname = f->first;
      f->second = (int) row[branchname];
      if ( Helpers::verbose() > 16 )
      {
        cout << "[RootInternalNtuple] ("
             << theName << ") " << branchname << "=" << f->second << endl;
      };
    };
    for ( map < string, char * >::iterator s=theStrings.begin();
          s!=theStrings.end() ; ++s )
    {
      string branchname = s->first;
      string data = s->second;
      if ( data.size() > theStringsSize[branchname] )
      {
        cout << "[RootInternalNtuple] warning: string literal ``" << data
             << "'' too long." << endl;
      };
      snprintf (s->second, theStringsSize[branchname],"%s",
               ((string) row[branchname]).c_str() );
      
      if ( Helpers::verbose() > 16 )
      {
        cout << "[RootInternalNtuple] ("
             << theName << ") " << branchname << "=" << s->second << endl;
      };
    };
    if ( Helpers::verbose() > 16 )
    {
      cout << "[RootInternalNtuple] Fill tree ``" << theName << "''" << endl;
    };
    theTree->Fill();
  };
  theData.clear();
  // firstWrite=true;
  // theDoubles.clear();
  // theInts.clear();
  // theStrings.clear();
  // theStringsSize.clear();
}

int RootInternalNtuple::nRows() const
{
  return theData.size();
}

void RootInternalNtuple::describe() const
{
  cout << "[RootInternalNtuple] \"" << name() << "\" contains " << nRows() << " rows." << endl;
  // cout << "   `- theHeader.size=" << theHeader.size() << endl;
  cout << "   `- doubles: ";
  for ( map < string, double >::const_iterator i=theDoubles.begin(); i!=theDoubles.end() ; ++i )
  {
    char * b = theTree->GetBranch ( i->first.c_str() )->GetAddress();
    cout << i->first << "(" << (void *) &(i->second) << "," << (void *) b << ") ";
  }
  cout << endl;
}

