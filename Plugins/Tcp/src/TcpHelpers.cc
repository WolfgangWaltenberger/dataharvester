#include <dataharvester/HarvestingConfiguration.h>
#include "TcpHelpers.h"
#include <dataharvester/HarvestingException.h>
#include <dataharvester/Helpers.h>
#include <netdb.h>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h> /* close */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/ioctl.h>

using namespace std;
using namespace dataharvester;

namespace {
  string correctURL()
  {
    return "Correct line should read: dhtp://server:port/filename";
  }
}

string TcpHelpers::readLine ( int skt )
{
  // read till newline
  char rcv_msg[2]="";
  ostringstream ret;

  while(1)
  {
    int n = recv( skt, rcv_msg, 1, 0 ); /* wait for data */
    if (n < 0) {
      /* if ( errno == EWOULDBLOCK )
      {
        cout << ret.str() << endl;
      };*/
      throw HarvestingException ( "Cannot receive data" );
    } else if (n == 0) {
      throw HarvestingException ( "Connection closed by client" );
    };
    string add = rcv_msg;
    if ( add == "\n" )
    {
      return ret.str();
    };
    ret << rcv_msg;
  }
}

string TcpHelpers::readBuffer ( int skt )
{
  char rcv_msg[2]="";
  ostringstream ret;

  while(1)
  {
    int n = recv( skt, rcv_msg, 1, 0); /* wait for data */
    if (n !=0) {
      return ret.str();
    };
    if ( errno )
    {
      cout << "[TcpHelpers] err! " << errno << endl;
      return ret.str();
    };
    ret << rcv_msg;
  }
}

string TcpHelpers::readWord ( int skt )
{
  // read till newline or space
  char rcv_msg[2]="";
  ostringstream ret;

  while(1)
  {
    int n = recv( skt, rcv_msg, 1, 0); /* wait for data */
    if (n < 0) {
      throw HarvestingException ( "Cannot receive data" );
    } else if (n == 0) {
      throw HarvestingException ( "Connection closed by client" );
    };
    string add = rcv_msg;
    if ( add == "\n" || add == " " )
    {
      return ret.str();
    };
    ret << rcv_msg;
  }
}

int TcpHelpers::sendPrompt ( int skt )
{
  return 0;
  string line="> ";
  int rc = send ( skt, line.c_str(), strlen ( line.c_str() )+1,0 );
  if(rc<0) {
    cout << "[TcpHelpers] could not send prompt!" << endl;
  };
  return rc;
}

int TcpHelpers::tcpSend ( int skt, string line )
{
  if ( line[line.size()] != '\n' )
  {
    line+="\n";
  };
  int rc = send ( skt, line.c_str(), strlen ( line.c_str() )+1,0 );
  if(rc<0) {
    cout << "[TcpHelpers] could not send data!" << endl;
  };
  return rc;
}

int TcpHelpers::acknowledge ( int skt, string comment )
{
  ostringstream snd;
  snd << "000";
  if ( comment.size() )
  {
    snd << " " << comment;
  } else {
    snd << " (no comment)";
  };
  return tcpSend ( skt, snd.str() );
}

int TcpHelpers::contactServer ( string url )
{
  return contactServer ( getHost ( url ), getPort ( url, false ) );
}

int TcpHelpers::contactServer ( string server, int port )
{
  struct sockaddr_in localAddr, servAddr;
  struct hostent *h;

  h = gethostbyname( server.c_str() );

  if (h == NULL)
  {
    throw HarvestingException ( (string) "unknown host " + server );
  }

  servAddr.sin_family = h->h_addrtype;
  memcpy((char *) &servAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
  servAddr.sin_port = htons( port );

  if (Helpers::verbose() > 0 )
    cout << "[TcpHelpers] Creating socket ... " << std::flush;
 
  /* create socket */
  int sckt = socket(AF_INET, SOCK_STREAM, 0);
  if( sckt < 0 ) {
    throw HarvestingException ( "Cannot open socket" );
  }

  if (Helpers::verbose() > 0 )
    cout << "binding ... " << std::flush;
  /* bind any port number */
  localAddr.sin_family = AF_INET;
  localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localAddr.sin_port = htons(0);

  int rc = bind( sckt, (struct sockaddr *) &localAddr, sizeof(localAddr));
  if( rc < 0 )
  {
    ostringstream ostr;
    ostr << "cannot bind tcp port " << port;
    throw HarvestingException ( ostr.str() );
  }

  if (Helpers::verbose() > 0 )
    cout << "connecting to " << inet_ntoa ( servAddr.sin_addr ) << ":"
         << servAddr.sin_port << " ... " << std::flush;
  /* connect to server */
  rc = connect( sckt, (struct sockaddr *) &servAddr, sizeof(servAddr));
  if( rc < 0 ) {
    cout << " cannot connect!" << endl;
    throw HarvestingException ( "cannot connect" );
  };
  if (Helpers::verbose() > 0 )
    cout << "we're hooked up!" << endl;
  return sckt;
}

void TcpHelpers::turnNonBlocking ( int sckt, int onoff )
{
  ioctl ( sckt, FIONBIO, &onoff );
}

string TcpHelpers::getHost ( string url )
{
  string line = stripDhtp ( url );
  string ret = "localhost";
  string::size_type pos = line.find( ":" );
  if ( (signed) pos == -1 )
  {
    pos = line.find( "/" );
    if ( (signed) pos == -1 )
    {
    };
  } else {
    ret = line.substr( 0, pos );
  };
  return ret;
}

int TcpHelpers::getPort ( string url, bool strict )
{
  string line = stripDhtp ( url );
  int ret = 23142;
  string::size_type pos = line.find( ":" );
  if ( (signed) pos != -1 )
  {
    string::size_type nd = line.find( "/" );
    if ( (signed) nd == -1 )
    {
      if ( strict )
      {
        cout << "[TcpHelpers] warning " << correctURL()
             << endl;
      } else {
        string sret = line.substr ( pos+1, line.size() );
        ret = atoi ( sret.c_str() );
      };
    } else {
      string sret = line.substr( pos+1, nd-pos-1 );
      ret=atoi ( sret.c_str() );
    };
  };
  return ret;
}

string TcpHelpers::getFileName ( string url )
{
  string line = stripDhtp ( url );
  if ( line.find ("/" ) == string::npos )
  {
    cout << "[TcpHelpers] Error: url ``" << url << "'' illegal." << endl;
    exit(-1);
  };
  return line.substr( line.find("/")+1, line.size() );
}

string TcpHelpers::stripDhtp ( string url )
{
  string ret=url;
  if ( ret.find ("dhtp://") != string::npos )
  {
    ret=url.substr ( 7, url.size()-7 );
  };
  return ret;
}

