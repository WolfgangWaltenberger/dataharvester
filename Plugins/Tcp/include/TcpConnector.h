#ifndef TcpConnector_H
#define TcpConnector_H

#include <string>

namespace dataharvester {
class TcpConnector {
public:
  /**
   *  \class TcpConnector
   *  Class that collects tcp (client side) connections.
   *  Makes sure that only one connection is opened up
   *  for every server/server_port/file combo.
   *  \param url dhtp://server:port/filename
   */
  int registerSocket ( std::string URL ); //< register new URL
  int getSocket ( std::string URL ); //< get right socket handle
  void closeSocket ( std::string URL ); //< close URL
  void closeSocket (); //< close all sockets

private:
  int tcpSend ( int socket, std::string line ) const; //< send std::string (to comm server)
  std::string readWord( int socket ) const; //< read a word (from comm server)
  std::string readLine( int socket ) const; //< read a line (from comm server)

  /// negotiate a data channel with the comm server
  /// returns port of data channel,
  /// -1 if unsuccessful
  int negotiateDataChannel ( int sckt, std::string filename ) const;

};
}

#endif
