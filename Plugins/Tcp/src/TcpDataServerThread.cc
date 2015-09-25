#include <dataharvester/HarvestingConfiguration.h>
#include "TcpDataServerThread.h"
#include <dataharvester/Writer.h>
#include "TcpHelpers.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/StreamableReader.h>
#include <dataharvester/ReaderManager.h>
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

TcpDataServerThread::TcpDataServerThread( int skt, string filename,
    unsigned short port, const sockaddr_in & client, int * ct ) :
  theSocket ( skt ), thePort ( port ), theFileName ( filename ),
  theCliAddr ( client ), theCounter ( ct )
{
  (*theCounter)++;
}

void TcpDataServerThread::sendHello()
{
  ostringstream answer;
  answer << "000 Helo " << inet_ntoa ( theCliAddr.sin_addr ) << ":" << theCliAddr.sin_port
         << ". This is Harvestingd $Rev: 214 $: data channel for " << theFileName << ".";
  TcpHelpers::tcpSend ( theSocket, answer.str() );
}

void TcpDataServerThread::interpret ( const string & line )
{
  // stringstream is;
  // is << line;
  StreamableReader * seeder = ReaderManager::self()->streamableReader ( "txt" );
  bool write = seeder->addLine ( line );
  if ( write )
  {
    const Tuple & ret = seeder->getCurrentTuple(); 
    Writer::file ( theFileName ) << ret;
  }
  TcpHelpers::acknowledge( theSocket );
}

void TcpDataServerThread::communicate()
{
  sendHello();
  try {
    while (1) { // as long as this socket has parsable data
      string r = TcpHelpers::readLine( theSocket );
      if ( r.empty() )
      {
        cout << "[TcpDataServerThread:" << thePort << "] empty line. Ignore it." << endl;
        continue;
      };
      if ( Helpers::verbose()>49 )
      {
        cout << "[TcpDataServerThread:" << thePort << "] r=``" << r << "''" << endl;
      };
      // r.replace ( r.size()-1,1,"" );
      if ( r == (string) "__END__" )
      {
        if ( Helpers::verbose()>49 )
        {
          cout << "[TcpDataServerThread:" << thePort << "] end marker found!" << endl;
        };
        Writer::close ( theFileName );
        return;
        // socket gets closed in destructor
      };
      if ( Helpers::verbose()>10 )
      {
        cout << "[TcpDataServerThread:" << thePort << "] Client "
             << inet_ntoa ( theCliAddr.sin_addr ) << ": ``" << r << "''" << endl;
      };
      interpret ( r ); // interpret command
    };
  } catch ( HarvestingException & e ) {
    cout << "[TcpDataServerThread:" << thePort << "] " << e.what() << endl;
    return;
  };
  return;
}

TcpDataServerThread::~TcpDataServerThread()
{
  theCounter--;
  close ( theSocket );
}

