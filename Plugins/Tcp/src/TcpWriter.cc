#include <dataharvester/HarvestingConfiguration.h>
#include "TcpWriter.h"
#include <dataharvester/Helpers.h>
#include "TcpHelpers.h"
#include <dataharvester/HarvestingException.h>
#include <dataharvester/SimpleConfigurable.h>
#include <dataharvester/Mode.h>
#include "TcpConnector.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <unistd.h>
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
  string correctLine()
  {
    return "Correct line should read: dhtp://server:port/filename";
  }

  int getPort ( string line )
  {
    int ret = 23142;
    string::size_type pos = line.find( ":" );
    if ( (signed) pos != -1 )
    {
      string::size_type nd = line.find( "/" );
      if ( (signed) nd == -1 )
      {
        cout << "[TcpWriter] warning " << correctLine()
             << endl;
      } else {
        string sret = line.substr( pos+1, nd-pos-1 );
        ret=atoi ( sret.c_str() );
      };
    };
    return ret;
  }

  string getHost ( string line )
  {
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

  string getRemoteFile ( string line )
  {
    string ret = "/dev/null";
    string::size_type nd = line.find( "/" );
    if ( (signed) nd == -1 )
    {
      cout << "[TcpWriter] warning " << correctLine()
           << endl;
    } else {
      ret = line.substr( nd );
    };
    return ret;
  }
}

TcpWriter & TcpWriter::operator<< ( const Tuple & d )
{
  cout << "[TcpWriter] not implemented" << endl;  
  return (*this);
}

void TcpWriter::save ( const Tuple & d, const string & prefix )
{
  const_cast <Tuple &> (d).fill ( "", false );
  cout << "[TcpWriter] not implemented" << endl;
}


void 
TcpWriter::save ( const map < string, MultiType > & mp, const string & n_n_d )
{
  ostringstream newstream;
  theOStreamer.setStream ( newstream, false );
  // theOStreamer.save( mp, n_n_d );
  if ( Helpers::verbose() > 2 )
  {
    cout << "[TcpWriter] tcp transfer to: " << destination() << " -> "
         << newstream.str().size() 
         << " characters." << endl;
  };
  string data = newstream.str();
  if ( data[data.size()-1]=='\n' )
  {
    data.replace ( data.size()-1, 1, "" );
  };
  if ( data == (string) "" ) return;
  int ctr=0;
  do {
    if ( Helpers::verbose() > 49 )
    {
      cout << "[TcpWriter] sending ``" << data << "''" << endl;
    };
    tcpSend ( data );
    ctr++;
    if ( ctr > 5 )
    {
      // tried five times. break;
      break;
    };
    // send as long as we dont have an acknowledgement
  } while ( !waitForAcknowledgment() );
}

string TcpWriter::destination() const
{
  ostringstream ostr;
  ostr << "dhtp://" << theServer << ":" << thePort << theRemoteFileName;
  return ostr.str();
}

TcpWriter::~TcpWriter()
{
  if ( Helpers::verbose() > 3 )
    cout << "[TcpWriter] sending close signal for "
         << theRemoteFileName << endl;
  bool send_endmarker=SimpleConfigurable<bool>(false,"TcpWriter:SetEndMarker").value();
  if ( send_endmarker )
  {
    cout << "[TcpWriter] sending end marker" << endl;
    ostringstream ostr;
    ostr << "__END__"; // this will close the file!!!
    tcpSend ( ostr.str() );
    string ret = readWord();
    readLine();
    if ( ret == "000" )
    {
      if ( Helpers::verbose() > 5 )
      {
        cout << "[TcpWriter] end confirmed." << endl;
      };
    } else {
      cout << "[TcpWriter] Error! end signal was not confirmed" << endl;
      cout << "[TcpWriter] we close socket anyways." << endl;
    }

    // waitForAcknowledgment();

    // while ( !waitForAcknowledgment() ) {};
  };

  // dont close too quickly
  TcpConnector().closeSocket ( destination() );
}

TcpWriter::TcpWriter ( const string & filename ) :
  theRemoteFileName ( "" ), 
  theServer ( "localhost" ), thePort ( theDefaultPort ),
  theOStreamer ( TextWriter ( "" ) )
{
  if ( filename == "" ) return;
  string line = filename;
  if ( line[0]!='/' || line[1]!='/' )
  {
    cout << "[TcpWriter] warning: " << correctLine() << endl;
  } else {
    line.replace(0,2,"");
  };
  theServer = getHost ( line );
  thePort = getPort ( line );
  theRemoteFileName = getRemoteFile ( line );
  TcpConnector().registerSocket ( destination() );
}

bool TcpWriter::waitForAcknowledgment() const
{
  // next 2 couts should only be visible on verbosity > 2 (FIXME ?)
  if ( Helpers::verbose() > 2 )
  {          
    cout << "[TcpWriter] waiting for acknowledgment" << endl;
  }
  // true = acknowledged
  try {
    string ret = readWord();
    if ( ret == "000" )
    {
      if ( Helpers::verbose() > 2 )
      {          
        cout << "[TcpWriter] acknowledgement received." << endl;
      };

      try
      {
        // now read out the rest of the line
        ret = readLine();
        {
          if ( Helpers::verbose() > 2 )
          {
            cout << "[TcpWriter] Server: ``" << ret << "''" << endl;
          }
        };
      } catch ( ... ) {};

      return true;
    };
    cout << "[TcpWriter] response was ``" << ret << "'' not ``000''" << endl;
  } catch ( HarvestingException & e )
  {
    cout << "[TcpWriter] Exception: " << e.what() << endl;
    return false;
  };

  // FIXME should read out pipe, then resend
  // read out pipe, then resend
  int tme = 2 + int ( 10. * drand48() );
  cout << "[TcpWriter] trying to resend data in " << tme << " seconds!" << endl;
  sleep( tme );
  while(1)
  {
    char rcv_msg[2]="";
    int n = recv( TcpConnector().getSocket ( destination() ), rcv_msg, 1, 0); /* wait for data */
    if (n < 1) break;
  };
  return false;
}

string TcpWriter::readLine() const
{
  return TcpHelpers::readLine ( TcpConnector().getSocket ( destination() ) );
}

string TcpWriter::readWord() const
{
  return TcpHelpers::readWord ( TcpConnector().getSocket ( destination() ) );
}

int TcpWriter::tcpSend ( string line ) const
{
  return TcpHelpers::tcpSend ( TcpConnector().getSocket ( destination() ), line );
}

TcpWriter * TcpWriter::clone( const string & filename ) const
{
  return new TcpWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < TcpWriter, WriterManager::NetworkProtocol > t ( "dhtp" );
}

