#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Mode.h>
#include <dataharvester/SimpleConfigurable.h>
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;
using namespace dataharvester;

namespace
{
  typedef Mode::FileMode FileMode;

  FileMode getDefaultFileMode()
  {
    static bool has_result=false;
    static FileMode ret = Mode::Recreate;
    if ( has_result ) return ret;
    string ss = SimpleConfigurable < string > 
      ( "Recreate", "Mode:FileMode" ).value();
    if ( ss == "Recreate" || ss == "recreate" )
    {
      ret = Mode::Recreate;
    } else if ( ss == "Update" || ss == "update" || ss=="Append"
                || ss=="append" )
    {
      ret = Mode::Update;
    } else {
      cout << "[Mode] Error unknown file mode ``"
           << ss << "''" << endl;
      cout << "[Mode] only ``Recreate'' and ``Append'' are known"
           << endl;
      exit (-1);
    };
    has_result=true;
    return ret;
  }

  FileMode myFileMode = getDefaultFileMode();
}

void Mode::setFileMode ( FileMode m )
{
  myFileMode = m;
}

FileMode Mode::fileMode()
{
  return myFileMode;
}

string Mode::fileModeString()
{
  if ( myFileMode == Mode::Recreate )
  {
    return "recreate";
  } else {
    return "update";
  };
}
