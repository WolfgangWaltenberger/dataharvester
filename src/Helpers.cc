#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/SimpleConfigurable.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace {
  int backup()
  {
    static int ret = SimpleConfigurable <int> (
        0, "Writer:Backup" ).value();
    return ret;
  }

  int myVerbosity = SimpleConfigurable <int> (
      0, "Writer:Verbose" ).value();
}

pair < string, int > Helpers::extractLength ( string s )
{
  string name=s;
  int lngth=-1;
  string::size_type pos = s.find("@");
  if ( pos != string::npos && pos > 0 && s[pos-1]!='\\' )
  {
    name=s.substr(0,pos);
    string tmp=s.substr(pos+1,s.size());
    lngth=atoi(tmp.c_str());
  };
  return pair < string, int > ( name, lngth );
}

int Helpers::verbose()
{
  return myVerbosity;
}

bool Helpers::wantsLengthInfo()
{
  static bool ret = SimpleConfigurable <bool> (
      false, "Helper:EncodeLengthInfo" ).value();
  return ret;
}


void Helpers::setVerbosity ( int v )
{
  myVerbosity = v;
}

bool Helpers::ignoreMetaData()
{
  static bool ret = SimpleConfigurable <bool> (
      true, "Reader:IgnoreMetaData" ).value();
  return ret;
}

bool Helpers::flatData()
{
  static bool ret = SimpleConfigurable <bool> (
      true, "Writer:FlatData" ).value();
  return ret;
}

int Helpers::maxStringLength()
{
  static int ret = SimpleConfigurable <int> (
      80, "Helper:MaxStringLength" ).value();
  return ret;
}

string Helpers::backupName ( const string & name )
{
  return "backup."+name;
}

void Helpers::createBackup ( const string & name, bool copy )
{
  if ( !backup() ) return;
  if ( copy )
  {
    ifstream ifs( name.c_str(), ios::binary);
    ofstream ofs( backupName(name).c_str(), ios::binary);
    ofs << ifs.rdbuf();
  } else {
    rename ( name.c_str(), backupName ( name ).c_str() );
  }
  /*
  string newname = name;
  string::size_type pos = newname.rfind(".");
  if ( (int) pos < (int) newname.size() )
  {
    // FIXME root-specific!!!! Argh!
    newname.replace ( pos,newname.size()-pos,".old.root" );
  } else {
    newname+=".old";
  };
  pos = newname.rfind("/");
  if ( (int) pos < (int) newname.size() )
  {
    newname.replace ( 0,pos,"" );
  };
  newname="/tmp/"+newname;
  if ( verbose() )
  {
    cout << "[Helpers] Backing up " << name << " -> " << newname 
         << endl;
  };
  rename ( name.c_str(), newname.c_str() );
  */
}

bool Helpers::checkTupleName ( const string & name )
{
  if ( name == "__comments__" )
  {
    return false;
  };
  return true;
}

bool Helpers::dontSave()
{
  return false;
}
