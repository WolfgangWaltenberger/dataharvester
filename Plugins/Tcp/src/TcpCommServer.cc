#include <dataharvester/HarvestingConfiguration.h>
#include "TcpCommServer.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/HarvestingException.h>
#include "TcpHelpers.h"
#include <dataharvester/Writer.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <csignal>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h> /* close */
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>
#include <string>

using namespace std;
using namespace dataharvester;

namespace {
  TcpCommServer * mySingletonServer = 0;
  int mySocket;
  int myDataPort;
  int myCommPort;
  struct sockaddr_in myCliAddr;
  map < int, TcpDataServer * > myDataChannels;
  vector < string > myCommandTokens;
  map < string, string > myCommandComments;
  map < string, string > myCommandArgs;
  map < string, void (*)(int) > myCommandFuncs;
  int myLongestCommandLine=0; // max length ( command + args), for formatting help output

  string clientIP ( bool port=false )
  {
    // just return the client IP
    ostringstream ans;
    ans << inet_ntoa ( myCliAddr.sin_addr );
    if ( port ) ans << ":" << myCliAddr.sin_port;
    return ans.str();
  }

  void checkDataChannels()
  {
    cout << "[TcpCommServer] check existing data channels" << endl;
    for ( map < int, TcpDataServer * >::iterator i=myDataChannels.begin(); 
          i!=myDataChannels.end() ; ++i )
    {
      if ( i->first == 0 ) continue;
      if ( i->second == 0 ) continue;
      if ( !(i->second->isRunning()) )
      {
        cout << "[TcpCommServer] " << i->first << " not running." << endl;
        delete i->second;
        myDataChannels[i->first]=0; // .erase ( i->first );
        myDataChannels.erase ( i->first );
        myDataPort=i->first; // recycle dead lines
      };
    };
  }

  int getNewPortNumber()
  {
    if ( myDataChannels.size() > 100 )
    {
      checkDataChannels();
    };
    myDataPort++;
    if ( ( myDataPort - myCommPort ) > 200 )
    {
      myDataPort = myCommPort + 1;
      while ( myDataChannels[myDataPort] )
      {
        myDataPort++;
      };
    };
    cout << "[TcpCommServer] open new data channel " << myDataPort
         << endl;
    return myDataPort;
  }

  void cleanUp()
  {
    static bool clean_once = true;
    if (! clean_once) return;
    if ( Helpers::verbose() > 0 )
      cout << "[TcpCommServer] cleanup" << endl;
    clean_once = false;
    if (mySingletonServer)
    {
      mySingletonServer->destroy();
      mySingletonServer = 0;
    } else {
      cout << "[TcpCommServer] Error: mySingletonServer not set!" << endl;
    };
    if ( Helpers::verbose() > 0 )
      cout << "[TcpCommServer] finished cleaning" << endl;
    exit(0);
  }

  void * connect_tcp_data_server ( void * srv )
  {
    // called by the threader
    int channel = (int) srv;
    if ( myDataChannels[channel]==0 )
    {
      cout << "[TcpCommServer] Error trying to connect to non-existent data channel!" << endl;
    } else {
      myDataChannels[channel]->tcpListen();
    };
    cout << "[TcpCommServer] exiting data channel:" << channel
         << " thread" << endl;
    delete myDataChannels[channel];
    myDataChannels.erase ( channel );
    pthread_exit(0);
  }

  void cleanUp( int t )
  {
    cleanUp();
    exit(0);
  }

  vector < string > splitIntoWords ( string o )
  {
    vector < string > ret;
    while ( o.find ( " " ) != string::npos )
    {
      string::size_type pos = o.find(" ");
      string tmp=o.substr(0,pos);
      ret.push_back ( tmp );
      o.replace(0,pos+1,"");
    };
    ret.push_back ( o );
    return ret;
  }

  void openTransfer ( int skt )
  {
    if ( myCommandTokens.size() < 1 )
    {
      TcpHelpers::tcpSend( skt, "103 syntax: open <file>" );
      return;
    };
    string filename = myCommandTokens[0];
    int datachannel=0;
    for ( map < int, TcpDataServer * >::const_iterator
          i=myDataChannels.begin(); i!=myDataChannels.end() ; ++i )
    {
      if ( i->second->fileName() == filename )
      {
        // datachannel exists already
        datachannel = i->first;
        break;
      };
    };
    if ( datachannel == 0 )
    {
      // open new data channel
      datachannel=getNewPortNumber();
      TcpDataServer *srv =
        new TcpDataServer ( datachannel, filename, myCliAddr );
      myDataChannels[datachannel]=srv;
      myDataChannels[datachannel]->connect();
      pthread_t thread1;
      pthread_create ( &thread1, 0,
                       &connect_tcp_data_server, (void *) datachannel );
    };

    ostringstream answer;
    answer << "003 " << datachannel << " " << filename
           << " (opening channel to " << filename << " on port " << datachannel << " for " << clientIP() << ")";

    TcpHelpers::tcpSend ( skt, answer.str() );
  }

  void unlinkFiles ( int skt )
  {
    if ( myCommandTokens.size() < 1 )
    {
      TcpHelpers::tcpSend( skt, "101 syntax: del <file1> <file2>" );
      return;
    };
    for ( vector< string >::const_iterator i=myCommandTokens.begin(); 
          i!=myCommandTokens.end() ; ++i )
    {
      int ret = unlink ( i->c_str() );
      if (ret == 0 )
      {
        TcpHelpers::tcpSend( skt, "002 removing " + (*i) + " succeeded" ); 
        break;
      };
      switch ( errno )
      {
        case EACCES:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - access denied" );
          break;
        case EBUSY:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - file busy" );
          break;
        case EFAULT:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - file outside accessible address space" );
          break;
        case EIO:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - I/O error" );
          break;
        case EISDIR:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - file is directory" );
          break;
        case ELOOP:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - too many symbolic links" );
          break;
        case ENAMETOOLONG:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - filename too long" );
          break;
        case ENOENT:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - does not exist" );
          break;
        case ENOMEM:
          TcpHelpers::tcpSend ( skt, "103 delete " + (*i) + " - insufficient kernel memory" );
          break;
        default:
          ostringstream oss;
          oss << "103 delete " << *i << ": error " << ret;
          TcpHelpers::tcpSend( skt, oss.str() );
          break;
      };
    };
  }

  void closeAllTransfers( int skt )
  {
    for ( map< int, TcpDataServer * >::iterator i=myDataChannels.begin(); 
          i!=myDataChannels.end() ; ++i )
    {
      if ( i->second == 0 ) continue;
      delete i->second;
      i->second=0;
      myDataChannels[i->first]=0;
      // myDataChannels.erase ( i->first );
      ostringstream answer;
      answer << "006 closed " << i->second->fileName() << ":" << i->first;
      TcpHelpers::tcpSend( skt, answer.str() );
    };
    myDataChannels.clear();
    ostringstream fles;
    fles << "027 closed " << Writer::listWriters().size() << " files.";
    Writer::close();
    TcpHelpers::tcpSend( skt, fles.str() );
    TcpHelpers::tcpSend( skt, "026 finished" );
  }
  
  void unimplemented(int skt )
  {
    TcpHelpers::tcpSend( skt, "109 unimplemented" );
  }

  void listChannels(int skt )  
  {
    // FIXME channel == file   
    ostringstream answer;
    answer << "205 list of open channels:\n";

    if ( myDataChannels.empty() )
    {
      answer << "    no open channels\n";
    };
    for ( map< int, TcpDataServer * >::const_iterator i=myDataChannels.begin();
          i!=myDataChannels.end() ; ++i )
    {
      answer << "   " << i->first << " - ";
      if ( i->second == 0 )
      {
        answer << "NULL";
      } else {
        answer << i->second->fileName();
        if ( i->second->isRunning() )
        {
          answer << " (running)";
        };
        if ( i->second->isDestroyed() )
        {
          answer << " (destroyed)";
        };
        answer << " " << i->second->numberOfClients() << " clients";
      };
      answer << "\n";
    };

    answer << "\n    list of open files:\n";
    vector < string > files=Writer::listWriters();

    if ( files.empty() )
      answer << "    no open files";
    for ( vector< string >::const_iterator i=files.begin(); i!=files.end() ; ++i )
    {
      answer << "       " << (*i);
    };
    TcpHelpers::tcpSend ( skt, answer.str() );
  }

  void closeTransfer ( int skt )
  {
    if ( myCommandTokens.size() < 1 )
    {
      TcpHelpers::tcpSend( skt, "103 syntax: close <file>" );
      return;
    };
    string filename = myCommandTokens[0];

    for ( map< int, TcpDataServer * >::const_iterator i=myDataChannels.begin(); 
          i!=myDataChannels.end() ; ++i )
    {
      if ( i->second == 0 ) continue;
      if ( i->second->fileName() == filename )
      {
        delete i->second;
        myDataChannels[i->first]=0;
        myDataChannels.erase ( i->first );
        ostringstream answer;
        answer << "006 closed " << filename << ":" << i->first;
        TcpHelpers::tcpSend( skt, answer.str() );
        Writer::close ( filename );
      };
    };
  }

  void sendHello(int skt)
  {
    ostringstream answer;
    answer << "000 Helo " << clientIP(true)
           << ". This is Harvestingd $Rev: 214 $ comm channel.";
    TcpHelpers::tcpSend ( skt, answer.str() );
  }

  void user(int skt)
  {
    ostringstream answer;
    if ( myCommandTokens.empty() )
    {
      answer << "125 user <username>";
    } else {
      answer << "050 Hello " << myCommandTokens[0] << endl;
    };
    TcpHelpers::tcpSend ( skt, answer.str() );
  }

  void pass(int skt)
  {
    ostringstream answer;
    if ( myCommandTokens.empty() )
    {
      answer << "125 pass <username>";
    } else {
      answer << "051 Login successful";
    };
    TcpHelpers::tcpSend ( skt, answer.str() );
  }


  void help(int skt)
  {
    ostringstream ans;
    ans << "001 commands are:\n";
    for ( map < string, string >::const_iterator 
          i=myCommandArgs.begin(); i!=myCommandArgs.end() ; ++i )
    {
      ans << "  " << i->first << " " << i->second 
          << setw ( myLongestCommandLine-i->first.size() - i->second.size() + 4 )
          << " -  " << myCommandComments[i->first] << "\n";
    };
    TcpHelpers::tcpSend ( skt, ans.str() );
  }

  // parse line, all communciation goes via skt
  void interpret ( string line, int skt )
  {
    vector < string > tokens = splitIntoWords ( line );
    if ( tokens.empty() )
    {
      return;
    };

    string cmd = tokens[0];
    tokens.erase ( tokens.begin() );
    myCommandTokens=tokens;
    if ( myCommandFuncs.count ( cmd ) )
    {
      myCommandFuncs[cmd]( skt );
    } else {
      TcpHelpers::tcpSend ( skt, "102 unknown command ``" + cmd + "''" );
    };
  }

  void * talkToClient( void * s )
  {
    int skt = (int) s;
    sendHello( skt );

    try {
      while (1) {
        TcpHelpers::sendPrompt(skt);
        string r = TcpHelpers::readLine( skt );
        if ( r.empty() )
        {
          cout << "[TcpCommServer] empty line. Ignore it." << endl;
          continue;
        };

        if ( r[r.size()]=='\0' )
        {
          r=r.substr(0,r.size()-1);
        };
        if ( r == (string) "exit" )
        {
          // exit is the only command that is handled here
          TcpHelpers::tcpSend( skt,"001 Byebye");
          close(skt);
          break;
        };
        if ( r == (string) "shutdown" )
        {
          // exit is the only command that is handled here
          TcpHelpers::tcpSend( skt,"005 Shutting down the server");
          close(skt);
          exit(0);
        };
        if ( Helpers::verbose()>10 )
        {
          cout << "[TcpCommServer] Client "
               << clientIP() << ":" << myCliAddr.sin_port 
               << " ``" << r << "''" << endl;
        };
        interpret ( r, skt ); // interpret command
      };
    } catch ( HarvestingException & e )
    {
      cout << "[TcpCommServer] " << e.what() << endl;
    };
    pthread_exit ( 0 );
  }

  void setupCommands()
  {
    myCommandArgs["open"]="<file>";
    myCommandComments["open"]="Opens file, returns port number of data channel";
    myCommandFuncs["open"]=openTransfer;

    myCommandArgs["helo"]="";
    myCommandComments["helo"]="Tell server to re-introduce itself";
    myCommandFuncs["helo"]=sendHello;

    myCommandArgs["user"]="<name>";
    myCommandComments["user"]="Set your username";
    myCommandFuncs["user"]=user;

    myCommandArgs["pass"]="<passwd>";
    myCommandComments["pass"]="Login as user with this password";
    myCommandFuncs["pass"]=pass;

    myCommandArgs["help"]="";
    myCommandComments["help"]="Show help";
    myCommandFuncs["help"]=help;

    myCommandArgs["list"]="";
    myCommandComments["list"]="Show open channels /open files";
    myCommandFuncs["list"]=listChannels;

    myCommandArgs["show"]="<file>";
    myCommandComments["show"]="Show status of file (unimplemented)";
    myCommandFuncs["show"]=unimplemented;

    myCommandArgs["exit"]="";
    myCommandComments["exit"]="Logout";

    myCommandArgs["shutdown"]="";
    myCommandComments["shutdown"]="Shutdown server";

    myCommandArgs["del"]="<files>";
    myCommandComments["del"]="Delete files on server";
    myCommandFuncs["del"]=unlinkFiles;

    myCommandArgs["close"]="<file>";
    myCommandComments["close"]="Flush data - close file on server.";
    myCommandFuncs["close"]=closeTransfer;

    myCommandArgs["close_all"]="";
    myCommandFuncs["close_all"]=closeAllTransfers;
    myCommandComments["close_all"]="Flush all data - close all files.";

    for ( map < string, string >::const_iterator i=myCommandArgs.begin(); 
        i!=myCommandArgs.end() ; ++i )
    {
      int tmp = i->first.size() + i->second.size();
      if ( tmp > myLongestCommandLine )
        myLongestCommandLine=tmp;
    };
  }
}

void TcpCommServer::initCleanup()
{
  int ret = atexit ( cleanUp );
  if ( ret != 0) {
    cout << "[TcpCommServer] registration of cleanup routine failed!" << endl;
    return;
  };
  signal ( SIGINT, cleanUp );
  signal ( SIGQUIT, cleanUp );
  signal ( SIGKILL, cleanUp );
}

TcpCommServer::TcpCommServer( unsigned short port ) :
  theOrigSocket ( 0 ),
  theStream(0), theDelStream ( false ),
  theIsDestroyed(false)
{
  myCommPort=port;
  mySocket=0;
  myDataPort=myCommPort;
  if ( mySingletonServer )
  {
    cout << "[TcpCommServer] Error: can run only once!" << endl;
    destroy();
    return;
  };
  mySingletonServer=this;
  setupCommands();
  theStream = &(std::cout);
  initCleanup();

  if ( Helpers::verbose() >= 1 )
  {
    (*theStream) << "[TcpCommServer] Starting to listen to port " << port << endl;
  };
  theOrigSocket = socket ( AF_INET, SOCK_STREAM, 0 );
  if ( theOrigSocket < 0 )
  {
    (*theStream) << "[TcpCommServer] Cannot open socket" << endl;
    return;
  };
  struct sockaddr_in servAddr;
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons( port );

  int attempts=0;
  static const int maxattempts=40;
  while ( bind(theOrigSocket, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0 )
  {
    (*theStream) << "[TcpCommServer] Cannot bind to port " << port << endl;
    if ( attempts > maxattempts )
    {
      (*theStream) << "[TcpCommServer] tried " << maxattempts << " times. Giving up."
                   << endl;
      exit(-1);
    };
    int tme = 3 + int ( 10. * drand48() );
    (*theStream) << "[TcpCommServer] will try again in " << tme << " seconds."
                 << endl;
    attempts++;
    sleep ( tme );
  };

  listen(theOrigSocket,5);

  (*theStream) << "[TcpCommServer] Now listening to port " << port << "." << endl;

  while(1) { // we listen and accept incoming connections
    if ( Helpers::verbose() >= 2 )
    {
      (*theStream) << "[TcpCommServer] Waiting for clients on port " << port << endl;
    };
    int cliLen = sizeof(myCliAddr);
    mySocket = accept( theOrigSocket, (struct sockaddr *) &myCliAddr, (socklen_t *) &cliLen);
    if(mySocket<0) {
      (*theStream) << "[TcpCommServer] Cannot accept connection" << endl;
      break;
    };

    pthread_t thread1;
    pthread_create ( &thread1, 0,
                     &talkToClient, (void *) mySocket );
  };
}

vector < string > TcpCommServer::commands()
{
  vector < string > ret;

  if (!myCommandArgs.size() )
  {
    setupCommands();
  };

  for ( map < string, string >::const_iterator i=myCommandArgs.begin();
        i!=myCommandArgs.end(); ++i )
  {
    ret.push_back ( i->first );
  };
  return ret;
}

void TcpCommServer::destroy()
{
  if ( theIsDestroyed )
  {
    cout << "[TcpCommServer] trying to destroy a dead TcpCommServer" << endl;
    return;
  };

  for ( map < int, TcpDataServer * >::const_iterator i=myDataChannels.begin(); 
        i!=myDataChannels.end() ; ++i )
  {
    delete i->second;
  };
  myDataChannels.clear();
  if ( theDelStream )
  {
    delete theStream;
    theStream=0;
    theDelStream=false;
  };
  if ( Helpers::verbose() > 2 )
  {
    cout << "[TcpCommServer] shutdown." << endl;
  };
  close ( mySocket );
  int ret = close(theOrigSocket);
  if ( ret == -1 )
  {
    cout << "[TcpCommServer] socket could not be closed " << errno << endl;
    switch (errno)
    {
      case EBADF:
        cout << "  " << mySocket << " isnt valid" << endl;
        break;
      case EINTR:
        cout << "  " << mySocket << " was interrupted" << endl;
        break;
      case EIO:
        cout << "  " << mySocket << " I/O error" << endl;
        break;
    };
  };
  theIsDestroyed=true;
  mySingletonServer=0;
  cout << "[TcpCommServer] closing all writers" << endl;
  Writer::close();
  cout << "[TcpCommServer] closed all writers" << endl;
}

TcpCommServer::~TcpCommServer()
{
  destroy();
  cout << "[TcpCommServer] destroyed" << endl;
}

