#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Cout.h>

using namespace std;
using namespace dataharvester;

namespace {
#if 1
  // static const char * kBlack = "\e[0;30m";
  static const char * kRed = "\e[0;31m";
  static const char * kGreen = "\e[0;32m";
  static const char * kYellow = "\e[0;33m";
  static const char * kBlue = "\e[0;34m";
  // static const char * kMagenta = "\e[0;35m";
  static const char * kCyan = "\e[0;36m";
  static const char * kReset = "\e[;0m";
#endif

#if 0
  // iso compliant
  // static const char * kBlack = "\e[0;30m";
  static const char * kRed = "";
  static const char * kGreen = "";
  static const char * kYellow = "";
  // static const char * kBlue = "\e[0;34m";
  // static const char * kMagenta = "\e[0;35m";
  static const char * kCyan = "";
  static const char * kReset = "";
#endif

  bool myUseColors=false;
}

void Cout::useColors ( bool onoff )
{
  myUseColors=onoff;
}

const char * Cout::colorCode ( int type )
{
  switch ( type )
  {
    case Info:
      return info();
    case Warning:
      return warning();
    case Error:
      return error();
    default:
      return "";
  };
}

const char * Cout::error( bool force_use )
{
  if ( force_use || myUseColors )
    return kRed;
  return "";
}

const char * Cout::special( bool force_use )
{
  if ( force_use || myUseColors )
    return kBlue;
  return "";
}


const char * Cout::warning( bool force_use )
{
  if ( force_use || myUseColors )
    return kGreen;
  return "";
}

const char * Cout::info( bool force_use )
{
  if ( force_use || myUseColors )
    return kYellow;
  return "";
}

const char * Cout::debug( bool force_use )
{
  if ( force_use || myUseColors )
    return kCyan;
  return "";
}

const char * Cout::reset( bool force_use )
{
  if ( force_use || myUseColors )
    return kReset;
  return "";
}

const char * Cout::newl( bool force_use )
{
  if ( force_use || myUseColors )
    return ((string) kReset + "\n").c_str();
  return "\n";
}
