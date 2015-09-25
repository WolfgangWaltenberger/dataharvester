#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/SystemWriter.h>
#include <dataharvester/Writer.h>
#include <dataharvester/Timer.h>
#include <iostream>
#include <iterator>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace dataharvester;

namespace {
  string readAndClose ( FILE * file )
  {
    static const int length=255;
    char tmpar[length]="";
    fread ( tmpar, length-1, 1, file );
    pclose ( file );
    return string ( tmpar );
  }
}

int SystemWriter::memory()
{
  try {
    string input="Unknown";
    {
      ifstream cpuinfo("/proc/meminfo");
      if ( (cpuinfo.is_open()) )
      {
        cpuinfo.unsetf( ios::skipws );
        istream_iterator<char> sbegin(cpuinfo),send;
        copy(sbegin,send,inserter(input,input.end()));
        cpuinfo.close();
      };
    };
  size_t i = input.find("MemTotal");
  if (i==string::npos)
  {
    // cout << "[SystemWriter] /proc/meminfo does not contain MemTotal." << endl;
    return 0;
  };
  i = input.find(":",i);
  return atoi(input.substr(i+1,input.find("kB",i)-i).c_str());
  } catch (...) {};
  return 0;
}

double SystemWriter::cpuMHz()
{
  return Timer::cpuMHz();
}

string SystemWriter::lsbRelease()
{
  try {
    FILE * tmp = popen ( "lsb_release -d -s", "r" );
    if ( tmp )
    {
      string ret=readAndClose ( tmp );
      if ( ret.find("\n") != string::npos )
      {
        ret=ret.substr(0,ret.find("\n") );
      }
      return ret;
    }
  } catch (...) {};
  return "???";
}

string SystemWriter::kernelName()
{
  try {
    FILE * tmp = popen ( "uname -s", "r" );
    if ( tmp )
    {
      string ret=readAndClose ( tmp );
      if ( ret.find("\n") != string::npos )
      {
        ret=ret.substr(0,ret.find("\n") );
      }
      return ret;
    }
    /*
    string input="??? ";
    {
      ifstream cpuinfo("/proc/version");
      if ( cpuinfo.is_open() )
      {
        cpuinfo.unsetf( ios::skipws );
        istream_iterator<char> sbegin(cpuinfo),send;
        copy(sbegin,send,inserter(input,input.end()));
        cpuinfo.close();
      }; 
    };
    try {
      input.replace( input.find("Linux version "),14, "" );
    } catch (...) {};
    size_t end = input.find("(");
    if ( end==string::npos)
    {
      return input;
    } else {
      return input.substr(0,end-1);
    };
    */
  } catch (...) {};
  return "???";
}

string SystemWriter::machineName()
{
  try {
    string input="127.0.0.1";
    {
      FILE * tmp = popen ( "hostname", "r" );
      if ( tmp )
      {
        input = readAndClose ( tmp );
        /*
      } else {
        // try again without fqdn!
        FILE * snd = popen ( "hostname", "r" );
        if ( snd )
        {
          input= readAndClose ( snd );
        }
        */
      }
    };
    size_t newl = input.find("\n");
    if ( newl!=string::npos )
    {
      input.replace(newl,1,"");
    };
    return input;
  } catch (...) {};
  return "Unknown machine";
}

string SystemWriter::currentTime()
{
  time_t tm = time ( 0 );
  string ret = ctime ( &tm );
  if ( (signed int) ret.find("\n") != -1 )
  {
    ret.replace ( ret.find("\n"),1,"" );
  };
  return ret;
}

SystemWriter::SystemWriter ( const string & filename,
    bool comments) :
    theFilename ( filename ), theComments ( comments ) {}

void SystemWriter::save ( int what, const string & name )
{
  Tuple attrs ( name );
  if ( what & CpuTime ) attrs["Cpu"]=cpuMHz();
  if ( what & Memory ) attrs["Memory"]=memory();
  if ( what & KernelName ) attrs["Kernel"]=kernelName();
  if ( what & MachineName ) attrs["Machine"]=machineName();
  if ( what & CurrentTime ) attrs["Time"]=currentTime();
  if ( what & LsbRelease ) attrs["linux"]=lsbRelease();
  if ( theComments )
  {
    attrs.describe("A tuple describing this machine");
    if ( what & CpuTime ) attrs.describe("CPU [MHz]","Cpu" );
    if ( what & Memory ) attrs.describe("Memory [KB]","Memory");
    if ( what & KernelName ) attrs.describe("Linux Kernel Version","Kernel");
    if ( what & MachineName ) attrs.describe("Hostname","Machine");
    if ( what & CurrentTime ) attrs.describe("Timestamp","Time");
  }
  Writer::file ( theFilename ) << attrs;
}

void SystemWriter::timestamp ( const string & ntuple, const string & column )
{
  Tuple attrs ( ntuple );
  attrs[column]=currentTime();
  Writer::file ( theFilename ).save ( attrs );
}
