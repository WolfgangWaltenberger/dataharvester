#include <dataharvester/HarvestingConfiguration.h>
#include "TcpDataServer.h"
#include "TcpDataServerThread.h"
#include <dataharvester/Writer.h>
#include "TcpHelpers.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/HarvestingException.h>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <csignal>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h> /* close */

using namespace std;
using namespace dataharvester;

namespace {
  void * thread_communicate ( void * tmp )
  {
    TcpDataServerThread * thrd = (TcpDataServerThread *) tmp;
    thrd->communicate();
    delete thrd;
    pthread_exit(0);
  }
}

int TcpDataServer::numberOfClients() const
{
  return theNClients;
}

TcpDataServer::TcpDataServer( unsigned short port, string file, 
                              const sockaddr_in & client ) :
  theNClients ( 0 ), theSocket(0), 
  theOrigSocket(0), theStream(0), theDelStream ( false ),
  theIsDestroyed(false), theIsRunning ( false), thePort ( port ),
  theFileName ( file )
{
  setup ( port, file, client );
}

TcpDataServer::TcpDataServer() : theNClients ( 0 ),
  theSocket ( 0 ), theOrigSocket ( 0 ), theStream ( 0 ),
  theDelStream ( false ), theIsDestroyed ( false ), 
  theIsRunning ( false ), thePort ( 0 ),
  theFileName ( "" )
{}

bool TcpDataServer::isRunning() const
{
  return theIsRunning;
}

bool TcpDataServer::isDestroyed() const
{
  return theIsDestroyed;
}


void TcpDataServer::setup ( unsigned short port, string file, const sockaddr_in & client )
{
  theFileName=file;
  if ( theFileName == (string) "" )
  {
    cout << "[TcpDataServer] Error: empty filename. Will save to -- instead"
         << endl;
    theFileName="--";
  };
  thePort=port;
  if ( thePort > 65535 )
  {
    cout << "[TcpDataServer] Error: illegal port " << thePort << endl;
  };
  theAllowedIP=client;
}

string TcpDataServer::fileName() const
{
  return theFileName;
}

int TcpDataServer::portNumber() const
{
  return thePort;
}

void TcpDataServer::connect()
{
  // in the data server mode we need to append
  // Mode::setFileMode ( Mode::Append );
  theStream = &(std::cout);

  if ( Helpers::verbose() >= 1 )
  {
    (*theStream) << "[TcpDataServer:" << thePort 
                 << "] Starting to listen to port " << thePort << endl;
  };
  theOrigSocket = socket ( AF_INET, SOCK_STREAM, 0 );
  if ( theOrigSocket < 0 )
  {
    (*theStream) << "[TcpDataServer:" << thePort << "] Cannot open socket" << endl;
    return;
  };
  struct sockaddr_in servAddr;
  // allow connection only from the allowed IP
  servAddr = theAllowedIP;
  servAddr.sin_port = htons( thePort );
  /*
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr = theAllowedIP;
  // servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  */

  while ( bind(theOrigSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0 )
  {
    (*theStream) << "[TcpDataServer:" << thePort << "] Cannot bind to port "
                 << thePort << endl;
    int tme = 2 + int ( 10. * drand48() );
    (*theStream) << "[TcpDataServer:" << thePort 
                 << "] will try again in " << tme << " seconds."
                 << endl;
    sleep ( tme );
  };

  listen(theOrigSocket,5);

  (*theStream) << "[TcpDataServer:" << thePort << "] Now listening on port "
               << thePort << " to "
               << inet_ntoa ( theAllowedIP.sin_addr ) << "." << endl;
  theIsRunning=true;
}

void TcpDataServer::tcpListen()
{
  while(1) { // we listen and accept incoming connections
    if ( theIsDestroyed )
    {
      cout << "[TcpDataServer] cannot listen anymore on destroyed object" << endl;
      return;
    };
    if ( Helpers::verbose() >= 2 )
    {
      (*theStream) << "[TcpDataServer:" << thePort << "] Waiting for clients on port "
                   << thePort << " (" << theIsDestroyed << ")" << endl;
    };
    int cliLen = sizeof(theCliAddr);
    theSocket = accept( theOrigSocket, (struct sockaddr *) &theCliAddr, (socklen_t *) &cliLen);
    if(theSocket<0) {
      (*theStream) << "[TcpDataServer:" << thePort
                   << "] Cannot accept connection" << endl;
      break;
    };
    TcpDataServerThread * thrd = new TcpDataServerThread
      ( theSocket, theFileName, thePort, theCliAddr, &theNClients );
    pthread_t thread1;
    pthread_create ( &thread1, 0,
                     &thread_communicate, (void *) thrd );
  };
}

void TcpDataServer::destroy()
{
  if ( theIsDestroyed )
  {
    cout << "[TcpDataServer:" << thePort << "] trying to destroy a dead TcpDataServer" << endl;
    return;
  };
  cout << "[TcpDataServer:" << thePort << "] destruction." << endl;
  if ( theDelStream )
  {
    delete theStream;
    theStream=0;
    theDelStream=false;
  };
  if ( theIsRunning && Helpers::verbose() > 2 )
  {
    cout << "[TcpDataServer:" << thePort << "] shutdown." << endl;
  };
  close(theSocket);
  theIsDestroyed=true;
  theIsRunning=false;
}

TcpDataServer::~TcpDataServer()
{
  destroy();
}

