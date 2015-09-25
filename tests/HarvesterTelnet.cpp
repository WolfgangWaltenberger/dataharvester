#include "HarvestingConfiguration.h"
#include "Helpers.h"
#ifdef WITH_TCP
#include "Writer.h"
// #include "TcpConnector.h"
#include "TcpHelpers.h"
#include "HarvestingException.h"
#include "TcpCommServer.h"
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

using namespace harvest;

extern char *xmalloc ( int );

// char * readline (const char *prompt);

namespace {
  void usage( string name )
  {
    cout << "Usage: " << name << " <url>" << endl;
  }

  vector < string > commands2;

  char * dupstr ( const char * s )
  {
    char *r;

    r = (char *) malloc (strlen (s) + 1);
    strcpy (r, s);
    return (r);
  }

  char * command_generator ( const char * text, int state )
  {
    static int list_index, len;
    const char *name;

    /* If this is a new word to complete, initialize now.  This includes
       saving the length of TEXT for efficiency, and initializing the index
       variable to 0. */
    if (!state)
    {
      list_index = 0;
      len = strlen (text);
    }

    int startindex=list_index;

    for ( vector< string >::const_iterator i=commands2.begin()+startindex; 
          i!=commands2.end() ; ++i )
    {
      name=i->c_str();
      
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    };

    return ((char *) NULL );

    /* Return the next name which partially matches from the command list. */

    while ( true ) // name = commands[list_index].name )
    {
      name = commands2[list_index].c_str();
      if ( name == "" ) return ((char *) NULL );
      list_index++;

      if (strncmp (name, text, len) == 0)
        return (dupstr(name));
    }

    /* If no names matched, then return NULL. */
    return ((char *)NULL);
  }

  char ** command_completion ( const char * text, int start, int end )
  {
    char ** matches;
    matches = rl_completion_matches (text, command_generator);
    return matches;
  }

  void initialize_readline()
  {
    commands2=TcpCommServer::commands();
    rl_readline_name = "HarvesterTelnet";
    rl_attempted_completion_function = command_completion;
  }
}

int main( int argc, char * argv[] )
{
  /**
   *  HarvesterTelnet
   *  readline-based program that makes interactive connections
   *  to a harvestingd more comfortable
   *
   */
  if ( argc < 2 )
  {
    usage ( argv[0] );
    exit(0);
  };

  string url = argv[1];

  int skt = TcpHelpers::contactServer ( url );

  string ret = TcpHelpers::readLine ( skt );
  cout << ret << endl << endl;
  if ( ret.find("000") != 0 )
  {
    cout << "[HarvesterTelnet] server did not answer correctly " << ret << endl;
    close ( skt );
    exit (-1);
  };

  TcpHelpers::turnNonBlocking ( skt, 1 );
  string prompt="\e[0;36mharvester:>\e[0;m ";
  initialize_readline ();
      
  while (true)
  {
    try
    {
      string cmd = readline ( prompt.c_str() );
      if ( cmd.find ( "exit" )==0 ) { break; };
      // cout << "[HarvesterTelnet] " << cmd << endl;
      add_history ( cmd.c_str() );
      cmd=cmd+" ";
      TcpHelpers::tcpSend ( skt, cmd );
      // FIXME quick hack to make it work
      // we wait a fixed time interval ...
      timespec ts;
      ts.tv_sec=0;
      ts.tv_nsec=100000000;
      nanosleep ( &ts, 0 );
      string ret = TcpHelpers::readLine ( skt );
      cout << ret << endl;
      try {
        while (1)
        {
          string ret = TcpHelpers::readLine ( skt );
          cout << ret << endl;
        };
      } catch ( HarvestingException & e )
      {
        // cout << "[HarvesterTelnet 2] " << e.what() << endl;
        // close ( skt );
        // exit(0); // this is a correct exit
      };
    } catch ( HarvestingException & e ) {
      // cout << "[HarvesterTelnet 1] " << e.what() << endl;
    };
  };
  close ( skt );
  exit(0);
}

#endif /* def WITH_TCP */
