#ifndef TcpHelpers_H
#define TcpHelpers_H

#include <string>

namespace dataharvester {
class TcpHelpers {
public:
  /**
   *  \class TcpHelpers
   *  Helper functions for tcp sockets
   */

  /** client connecting to server
   *  returns socket
   */
  static int contactServer ( std::string server, int port );
  static int contactServer ( std::string url );
  static void turnNonBlocking ( int sckt, int onoff );

  /// if line doesnt terminate with newline,
  /// it's added
  static int tcpSend ( int skt, std::string line );
  static int sendPrompt ( int skt );

  /// readWord and readLine throw HarvestingExceptions if the stream ends.
  static std::string readWord ( int skt ); //< terminating newline/space is removed
  static std::string readLine ( int skt ); //< terminating newline is removed
  static std::string readBuffer ( int skt ); //< terminating newline is removed
  static int acknowledge ( int skt, std::string comment="" );


  /// URL parsing routines
  static std::string getHost ( std::string url );
  static int getPort ( std::string url, bool strictUrlCheck=true );
  static std::string getFileName ( std::string url );
  static std::string stripDhtp ( std::string url );
};
}

#endif
