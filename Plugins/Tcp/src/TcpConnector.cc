#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/HarvestingException.h>
#include "TcpConnector.h"
#include <dataharvester/Helpers.h>
#include "TcpHelpers.h"
#include <map>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close */

using namespace std;
using namespace dataharvester;

namespace {
  string getHostAndPort ( string url )
  {
    string server = TcpHelpers::getHost ( url );
    int port = TcpHelpers::getPort ( url );
    ostringstream ostr;
    ostr << server << ":" << port;
    return ostr.str();
  }

  string getHostAndPortAndFilename ( string url )
  {
    string hap = getHostAndPort ( url );
    string f = TcpHelpers::getFileName ( url );
    return hap + "/" + f;
  }

  int debug()
  {
    return 1;
  }

  map < string, int > mySockets;
  map < string, int > countSockets;
}

int TcpConnector::negotiateDataChannel ( int sckt, string filename ) const
{
  try {
    if ( debug() )
    {
      cout << "[TcpConnector] now negotiate with comm server" << endl;
    };
    string answer = readWord ( sckt );

    if ( debug() )
    {
      cout << "[TcpConnector] comm server says " << answer << endl;
    };

    if ( answer == (string) "000" )
    {
      if ( debug() )
      {
        cout << "[TcpConnector] great! now read the line, then send the request" << endl;
      };
    } else {
      cout << "[TcpConnector] comm server says " << answer << "." << endl;
      cout << "[TcpConnector] who am I takling to?" << endl;
      return -1;
    };
    answer = readLine ( sckt );

    if ( debug() )
    {
      cout << "[TcpConnector] comm server: ``" << answer << "''" << endl;
    };

    // space is needed because filename ends in a NULL character ?!?
    tcpSend ( sckt, "open "+filename+" " );

    if ( debug() )
    {
      cout << "[TcpConnector] open " << filename << " sent" << endl;
    };
    answer = readWord ( sckt );

    if ( debug() )
    {
      cout << "[TcpConnector] comm server: ``" << answer << "''" << endl;
    };
    if ( answer == (string) "003" )
    {
      if ( debug () )
      {
        cout << "[TcpConnector] perfect! Now get the data channel port number!" << endl;
      };
    } else {
      cout << "[TcpConnector] Error " << answer << " upon trying to open file." << endl;
      cout << "[TcpConnector] Sorry, no socket." << endl;
      return -1;
    };

    answer = readWord ( sckt );

    if ( debug() )
    {
      cout << "[TcpConnector] comm server: ``" << answer << "''" << endl;
    };

    int port=atoi ( answer.c_str() );
    if ( port < 1 || port > 65535 )
    {
      cout << "[TcpConnector] " << answer << " is not a legitimate port number. Sorry." << endl;
      return -1;
    };
    return port;
  } catch ( HarvestingException & e ) {
    cout << "[TcpConnector] exception " << e.what() << endl;
    return -1;
  };
}

int TcpConnector::registerSocket( string url )
{
  string server_and_port = getHostAndPort ( url );
  string filename = TcpHelpers::getFileName ( url );
  string fullname = server_and_port + "/" + filename;

  // the attempt to register is always counted,
  // so we know when to cut the connection
  countSockets[fullname]++;
  if ( mySockets[fullname]  )
  {
    return mySockets[fullname];
  };

  if ( debug() )
  {
    cout << "[TcpConnector] registering " << server_and_port
         << " " << filename << endl;
  };

  string server = TcpHelpers::getHost ( url );
  int port = TcpHelpers::getPort ( url );

  if (Helpers::verbose() > 0 )
    cout << "[TcpConnector] connecting to " << server_and_port
         << endl;

  int sckt = TcpHelpers::contactServer ( server, port );
  int data_port = negotiateDataChannel ( sckt, filename );
  if ( debug() )
  {
    cout << "[TcpConnector] now connect to data channel " << server << ":" << data_port
         << endl;
  };
  int datasckt = TcpHelpers::contactServer ( server, data_port );
  mySockets[fullname]=datasckt;
  cout << "[TcpConnector] registered " << datasckt << " for " << fullname << endl;
  // TcpHelpers::turnNonBlocking ( datasckt, 1 ); // FIXME needed?
  return datasckt;
}

int TcpConnector::tcpSend ( int sckt, string line ) const
{
  return TcpHelpers::tcpSend ( sckt, line );
}

string TcpConnector::readWord( int sckt ) const
{
  return TcpHelpers::readWord ( sckt );
}


void TcpConnector::closeSocket ( string url )
{
  string server_and_port = getHostAndPort ( url );
  // only close when the last file in the connection
  // gets closed
  if ( --countSockets[server_and_port] == 0 )
  {
    if ( debug() )
    {
      cout << "[TcpConnector] server_and_port " << url << endl;
    };
    cout << "[TcpConnector] closing " << server_and_port << endl;
    close ( mySockets[server_and_port] );
  };
}

void TcpConnector::closeSocket ()
{
  for ( map< string, int >::const_iterator i=mySockets.begin();
        i!=mySockets.end() ; ++i )
  {
    close ( i->second );
  };
}

int TcpConnector::getSocket ( string url )
{
  string fullname = getHostAndPortAndFilename ( url );
  int ret = mySockets[fullname];
  if (!ret)
  {
    cout << "[TcpConnector] Error! no socket for " << fullname << " registered!" 
         << endl;
    exit(-1);
  };
  /* if ( debug() )
  {
    cout << "[TcpConnector] get handle for ``" << fullname << "'': " << ret << endl;
  }; */
  return ret;
}

string TcpConnector::readLine( int sckt ) const
{
  return TcpHelpers::readLine ( sckt );
}

