#include "RootHelpers.h"
#include <dataharvester/TupleRow.h>
#include <dataharvester/SimpleConfigurable.h>
#include <TLeaf.h>
#include <TTreePlayer.h> // ugly hack
#include <TBranch.h>
#include <TTree.h>
#include <TEnv.h>
#include <sstream>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace {

  TTree * myCurTree;
  
  void redirect ()
  {
    if ( !myCurTree ) return;
    TTreePlayer * p = (TTreePlayer * ) myCurTree->GetPlayer();
    p->SetScanRedirect(1);
    p->SetScanFileName ( "/dev/null" );
  }

  void addToArray ( vector < map < string, MultiType > > & ret,
      const string & branchname, const MultiType & data, int ctr, int row )
  {
    // ok, this is an array! we can treat it
    // as n variables, like variable1, variable2, ... variablen
    // or a variable[1], variable[2], ..., variable[n]
    // or as a true dataharvester array:
    // variable=value1,value2,value3.
    enum ArrayTreatment { Variables=1, Brackets=2, TrueArray=3 };

    ArrayTreatment a = TrueArray; // Variables;

    switch (a)
    {
      case Variables:
      {
        ostringstream o;
        o << branchname << ctr;
        ret[row][o.str()]=data;
        break;
      }
      case Brackets:
      {
        ostringstream o;
        o << branchname << "[" << ctr << "]";
        ret[row][o.str()]=data;
        break;
      }
      case TrueArray:
      {
        // ret[row][branchname].addToVector ( data );
        MultiType tmp = ret[row][branchname];
        // cout << endl << "[RootHelpers] heres the mt before: " << tmp << endl;
        tmp.addToVector ( data );
        ret[row][branchname]=tmp;
        // cout << endl << "[RootHelpers] heres the mt after: " << tmp << endl;
        break;
      }
    }
  }

  // add a vector to the vectormap!
  // FIXME this code is horrible!
  void addVector ( const vector < MultiType > & data,
                 vector < map < string, MultiType > > & ret,
                 string & branchname )
  {
    /*
    cout << "[RootHelpers] addVector: "
         << branchname << " " << data.size() << endl;
         */

    if ( ret.size() == 0 )  ret.push_back ( map < string, MultiType >() );
    if ( data.size() == 1 )
    {
      ret[0][branchname]=*(data.begin());
      return;
    }

    // how many matrix elements? -1 = not a matrix!
    int n_matrix_els=-1;

    string::size_type begin=branchname.find("[");
    string::size_type end=branchname.find("]");
    if ( begin != string::npos && end != string::npos )
    {
      string nels=branchname.substr(begin+1,end-begin);
      n_matrix_els=atoi ( nels.c_str() );
      // cout << "[RootHelpers] nel=" << n_matrix_els << endl;
      branchname=branchname.replace ( begin, end-begin+1,"" );
    }

    int ctr=0;
    unsigned row=0;
    // now copy the data of this sub branch into the right line
    // of 'ret'
    for ( vector< MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
    {
      if ( ret.size() < (row+1) )  ret.push_back ( map < string, MultiType >() );
      if ( branchname=="" )
      {
        cout << "[RootHelpers] Argh! branchname empty!" << endl;
        branchname="**noname**";
      }
      if (n_matrix_els > 0 )
      {
        // cout << "[hlprs] adding matrix elements!" << endl;
        addToArray ( ret, branchname, (*i), ctr, row );
      } else {
        ret[row][branchname]=(*i);
      }
      if ( false )
      {
        cout << " ::getNested: " << ctr << ": " << branchname << "=" << i->asString()
             //<< " offset=" << bsub->GetOffset() << ", " << bsub->GetEntryOffsetLen()
             << endl; 
      }
      ctr++;
      if ( n_matrix_els < 0 )
      {
        row++;
        ctr=0;
      } else {
        if ( ctr >= n_matrix_els )
        {
          ctr=0;
          row++;
        }
      }
    }
    // cout << "[RootHelpers] /addVector!" << endl;
  }

  // this is how it was done!
  void addVectorOld ( const vector < MultiType > & data,
                 vector < map < string, MultiType > > & ret,
                 string & branchname )
  {
    int ctr=0;
    // now copy the data of this sub branch into the right line
    // of 'ret'
    for ( vector< MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
    {
      if ( ret.size() < (ctr+1) )  ret.push_back ( map < string, MultiType >() );
      if ( branchname=="" )
      {
        cout << "[RootHelpers] Argh! branchname empty!" << endl;
        branchname="**noname**";
      }
      ret[ctr][branchname]=(*i);
      if ( false )
      {
        cout << " ::getNested: " << ctr << ": " << branchname << "=" << i->asString()
             //<< " offset=" << bsub->GetOffset() << ", " << bsub->GetEntryOffsetLen()
             << endl; 
      }
      ctr++;
    }
  }
}

MultiType RootHelpers::getValue ( const TLeaf * leaf, int entry )
{
  string type = leaf->GetTypeName();
  return getValue ( leaf, type, entry );
}

MultiType RootHelpers::typeName ( const string & type )
{
  ostringstream o;
  o << "**" << type << "**";
  return MultiType ( o.str() );
}

MultiType RootHelpers::getValue ( const TLeaf * leaf, const string & type, int entry, int offset )
{
  if ( type.find("edm::Wrapper") != string::npos )
  {
    return typeName ( type );
  }

  string name = leaf->GetName();
  int n = entry;
  void * vp = leaf->GetValuePointer();
  if ( type == "Char_t" )
  {
    if ( vp == NULL )
    {
      // should be a "char", but in real life it's charge, and
      // it has values -1 and +1 ....
      return MultiType ( (signed) leaf->GetValue ( n ) );
    }
    // must be a string
    string value = (char *) (vp);
    return MultiType ( value );
  } else if ( type == "Int_t" || type == "Bool_t" || type == "UInt_t" ||
              type == "UShort_t" || type == "ULong_t" || type == "ULong64_t" ) {
    // make sure an int remains an int
    // long value = atol ( t[sprefix+branchname].asString().c_str() );
    long value = (long) leaf->GetValue ( n );
    // cout << endl << "[RDS debug] int!" << value << endl;
    return MultiType ( value );
  } else if (  type == "Char_t" || type=="UChar_t" ) {

    if ( true )
    {
      // "characters are ints"
      long value = (long) leaf->GetValue ( n ) ;
      return MultiType ( value );
    } else {
      // "characters are strings"
      char value = (char) leaf->GetValue ( n );
      string s;
      s=value;
      return MultiType ( s );
    }
    /*
  } else if ( type == "edm::RefBase<unsigned int>" ) {
    long value = (long) leaf->GetValue ( entry );
    // cout << endl << "[RDS debug] int!" << value << endl;
    return MultiType ( value );
    */
  } else if ( type == "Double_t" || type=="Double32_t" )
  {
    // make sure a double remains a double
    // double value = atof ( t[sprefix+branchname].asString().c_str() );
    double value = leaf->GetValue ( n );
    return MultiType ( value );
  } else if ( type == "Float_t" || type== "Float32_t" )
  {
    // make sure a float remains a float
    float value = leaf->GetValue ( n );
    return MultiType ( value );
  } else if ( type == "vector<long>" ) {
    MultiType ret;
    vector < long > * t = new vector < long > ( );
    leaf->GetBranch()->SetAddress ( &t );
    leaf->GetBranch()->GetEntry( offset );
    for ( vector< long >::const_iterator i=t->begin(); i!=t->end() ; ++i )
    {
      ret.addToVector ( *i );
    }
    // cout << "[RootHelpers] t=" << t->size() << endl;
    delete t;
    return ret;
  } else if ( type == "vector<double>" ) {
    MultiType ret;
    vector < double > * t = new vector < double > ( );
    leaf->GetBranch()->SetAddress ( &t );
    leaf->GetBranch()->GetEntry( offset );
    for ( vector< double >::const_iterator i=t->begin(); i!=t->end() ; ++i )
    {
      ret.addToVector ( *i );
    }
    // cout << "[RootHelpers] t=" << t->size() << endl;
    delete t;
    return ret;
  } else {
    cout << "[RootHelpers] Error: cannot handle " << typeName ( type ).asString() << endl;
    return typeName ( type );
  }
}

vector < MultiType > RootHelpers::getValues ( const TLeaf * leaf, int offset )
{
  /*
  if ( leaf->GetLen() > 1 )
  {
    cout << "[RootHelpers] getValues:: " << leaf->GetName() << " offset=" << offset << endl;
    cout << "    `--- " << leaf->GetTypeName() << endl;
    cout << "    `--- " << leaf->GetLen() << endl;
    cout << "    `--- " << leaf->GetNdata() << endl;
  }*/
  // myCurTree->Scan ( leaf->GetName() );
  vector < MultiType > ret;
  int n = leaf->GetNdata() / leaf->GetLen();
  string type = leaf->GetTypeName();

  leaf->GetBranch()->GetEntry(offset );
 
  for ( int i=0; i< n ; i++ )
  {
    MultiType mt = getValue ( leaf, type, i, offset );
    ret.push_back ( mt );
  }

  return ret;
}

vector < MultiType > RootHelpers::getValues ( TBranch * branch, int entry )
{
  // cout << "[RootHelpers] getValues " << branch->GetName() << endl;
  // if subbranches exist, return nothing?
  int nsub=branch->GetListOfBranches()->GetEntries();
  if ( nsub > 0 ) return vector < MultiType > ();

  TObjArray * list = branch->GetListOfLeaves();
  if ( !list )
  {
    cout << "[RootHelpers] error: branch has no list of leaves!" << endl;
    return vector < MultiType > ();
  }

  int nleaves = list->GetEntries();
  if ( nleaves == 0 )
  {
    /*
    cout << "[RootHelpers] WARNING! Branch " << branch->GetName() << " has no leaves!"
         << endl;
         */
    return vector < MultiType > ();
  }

  vector < MultiType > ret;
  for ( int i=0; i< nleaves ; i++ )
  {
    TLeaf * leaf = myCurTree->GetLeaf ( list->At(i)->GetName() );
    if ( !leaf )
    {
      continue;
    }
    vector < MultiType > tmp= RootHelpers::getValues ( leaf, entry );
    for ( vector< MultiType >::const_iterator t=tmp.begin(); t!=tmp.end() ; ++t )
    {
      ret.push_back ( *t );
    }
  }
  return ret;
}

void RootHelpers::addData ( Tuple & t, const vector < MultiType > & data, 
                            unsigned row, const string & name )
{
  if ( row < data.size() )
  {
    t[name]=data[row];
  } else {
    t[name]=data[0];
  }
}

string RootHelpers::getShortLeafname ( TLeaf * branch, const string & prefix )
{
  string branchname = branch->GetName();
  // cout << "[RootHelpers] branchname " << branchname << endl;
  string::size_type pos = branchname.find(prefix);

  if ( prefix.size() && pos!=string::npos )
  {
    // cout << " -- remove: " << prefix << " from " << branchname << endl;
    branchname=branchname.replace (  pos, prefix.size(), "" );
    if ( branchname.size() && branchname[0]=='.' )
    {
      branchname=branchname.replace (0,1,"");
    }
  }
  // cout << "[RootHelpers]    ---->   " << branchname << endl;
  return branchname;
}

string RootHelpers::getShortBranchname ( TBranch * branch, const string & prefix )
{
  string branchname = branch->GetName();
  // cout << "[RootHelpers] branchname " << branchname << endl;
  string::size_type pos = branchname.find(prefix);

  if ( prefix.size() && pos!=string::npos )
  {
    // cout << " -- remove: " << prefix << " from " << branchname << endl;
    branchname=branchname.replace (  pos, prefix.size(), "" );
    if ( branchname.size() && branchname[0]=='.' )
    {
      branchname=branchname.replace (0,1,"");
    }
  }
  // cout << "[RootHelpers]    ---->   " << branchname << endl;
  return branchname;
}

void RootHelpers::printNested ( const vector < map < string, MultiType > > & m )
{
  cout << endl << "DataSet:" << endl;
  for ( vector< map < string, MultiType > >::const_iterator row=m.begin(); row!=m.end() ; ++row )
  {
    cout << "-----" << endl;
    for ( map < string, MultiType >::const_iterator data=row->begin(); data!=row->end() ; ++data )
    {
      cout << data->first << "=" << data->second.asString() << "; ";
    }
    cout << endl;
  }
}

vector < map < string, MultiType > > RootHelpers::getNested ( TBranch * branch,
    const string & prefix, int offset )
{
  vector < map < string, MultiType > > ret;

  int nsubbrs = branch->GetListOfBranches()->GetEntries();
  string supername = branch->GetName();

  int nsubleaves = branch->GetListOfLeaves()->GetEntries();

  if ( nsubleaves == 1 )
  {
    for ( int sub=0; sub< nsubbrs  ; sub++ )
    {
      // add branch by branch
      TBranch * bsub = (TBranch * ) (branch->GetListOfBranches()->At(sub));

      int bsubsubnrs=bsub->GetListOfBranches()->GetEntries();

      if ( bsubsubnrs > 0 )
      {
        continue; // dont get the sub-nested ones, here!
      }
      string branchname = getShortBranchname ( bsub, supername );

      vector < MultiType >  data = getValues ( bsub, offset );

      addVector ( data, ret, branchname );
    }
  } else {
    /* cout << "[RootHelpers] branch " << branch->GetName() << " has "
         << nsubleaves << " leaves!" << endl; */
    for ( int sub=0; sub<nsubleaves; sub++ )
    {
      // add leaf by leaf
      TLeaf * lsub = (TLeaf * ) (branch->GetListOfLeaves()->At(sub));
      string leafname = getShortLeafname ( lsub, supername );
      vector < MultiType >  data = getValues ( lsub, offset );
      addVector ( data, ret, leafname );
    }
  }
  return ret;
}

void RootHelpers::setCurTree ( TTree * t )
{
  if ( t == myCurTree ) return;
  myCurTree=t;
  redirect();
}

TTree * RootHelpers::curTree()
{
  return myCurTree;
}

void RootHelpers::ignoreWarnings()
{
  bool ignore = SimpleConfigurable <bool> (true, "Root:IgnoreWarnings").value();
  if ( ignore )
  {
    gEnv->SetValue("Root.ErrorIgnoreLevel","Error");
  }
}

