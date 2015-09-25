#include "HarvestingConfiguration.h"
#include "Helpers.h"
#ifdef WITH_TCP
#include "Writer.h"
#include "TcpConnector.h"
#include "TcpHelpers.h"
#include "HarvestingException.h"
#include <sstream>
#include <time.h>

using namespace harvest;

namespace {
  void usage( string name )
  {
    cout << "Usage: " << name << " <url> <cmd>" << endl;
  }
}

int main( int argc, char * argv[] )
{
  if ( argc < 3 )
  {
    usage ( argv[0] );
    exit(0);
  };
  string url = argv[1];

  int skt = TcpHelpers::contactServer ( url );
  try
  {
    string ret = TcpHelpers::readWord ( skt );
    if ( ret == "000" )
    {
      ret = TcpHelpers::readLine ( skt );
      cout << ret << endl << endl;
      ostringstream cmd;
      for ( int i=2; i< argc ; i++ )
      {
        cmd << argv[i] << " ";
      };
      TcpHelpers::tcpSend ( skt, cmd.str() );
      string ret = TcpHelpers::readLine ( skt );
      cout << ret << endl;
      TcpHelpers::turnNonBlocking ( skt, 1 );
      try {
        while (1)
        {
          string ret = TcpHelpers::readLine ( skt );
          cout << ret << endl;
        };
      } catch ( HarvestingException & e )
      {
        // cout << "[HarvesterCommand] " << e.what() << endl;
        close ( skt );
        exit(0);
        // this is a correct exit
      };
    } else {
      cout << "[HarvesterCommand] server did not answer correctly " << ret << endl;
    };
  } catch ( HarvestingException & e ) {
    // cout << "[HarvesterCommand] " << e.what() << endl;
  };
  close ( skt );
  exit(0);
}

#endif /* def WITH_TCP */
