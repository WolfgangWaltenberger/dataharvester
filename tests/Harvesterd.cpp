#include "HarvestingConfiguration.h"
#include "HarvestingHelpers.h"
#ifdef WITH_TCP
#ifdef WITH_SEEDER
#ifdef WITH_HARVESTER
#include "HarvestingCout.h"
#include "DataHarvester.h"
#include "TcpDataSeeder.h"
#include "TcpCommServer.h"
#include <unistd.h>

using namespace harvest;

namespace {
  unsigned short defaultport = 23142;
  void usage( string name )
  {
    cout << "Usage: " << name << " -h -c -p <port> -v <level> -f <file> -d <path>" << endl;
    cout << endl;
    cout << "  -h         Help" << endl;
    cout << "  -c         Enable color mode" << endl;
    cout << "  -p <port>  Port number to listen to (default "
         << defaultport << ")" << endl;
    cout << "  -v <level> Verbosity (default 1)" << endl;
    cout << "  -f <file>  Logfile (default: stdout)" << endl;
    cout << "  -d <path>  Rootpath for all generated files (default: \"./\")" 
              << endl;
  };
};

int main( int argc, char * argv[] )
{
  unsigned short port = defaultport;
  int verbosity = 0;
  bool colors = false;
  string filename="";
  string rootpath = "./";
  while (1)
  {
    int ret = getopt ( argc, argv, "hcp:v:f:d:" );
    if ( ret == -1 ) break;
    switch (ret)
    {
      case 'h': { usage(argv[0]); exit(0); };
      case 'c': { colors=true; break; };
      case 'p': { port = atoi ( optarg ); break; };
      case 'v': { verbosity = atoi ( optarg ); break; };
      case 'f': { filename = optarg; break; };
      case 'd': { rootpath = optarg; break; };
      default: 
        { cout << "[harvestingd] unknown option " << (char) ret << endl; };
    };
  };
  Cout::useColors ( colors );
  Helpers::setVerbosity( verbosity );
  // TcpDataSeeder seeder ( port, filename, rootpath );
  TcpCommServer server ( port );
};

#endif /* def WITH_TCP */
#endif /* def HARVESTER */
#endif /* def SEEDER */
