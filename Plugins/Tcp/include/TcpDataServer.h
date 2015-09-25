#ifndef TcpDataServer_H
#define TcpDataServer_H

#include <string>
#include <iostream>
#include <utility>
#include <arpa/inet.h>

namespace dataharvester {
class TcpDataServer {
public:
  /**
   *  \class TcpDataServer
   *  The server class of the communication channel
   */

  TcpDataServer ();
  TcpDataServer ( unsigned short port, std::string file,
                  const sockaddr_in & client ); 
  ~TcpDataServer();
  void destroy(); // close down comm line
  void setup ( unsigned short port, std::string file, const sockaddr_in & client );
  std::string fileName() const;
  int portNumber() const;
  void connect(); // connect to port
  void tcpListen(); // listen for data

  int numberOfClients() const;

  bool isRunning() const;
  bool isDestroyed() const;

private:
  int theNClients;
  int theSocket;
  int theOrigSocket;
  std::ostream * theStream; // messages
  bool theDelStream; // do we need to delete theStream?
  bool theIsDestroyed; // comm line closed already?
  bool theIsRunning;
  int thePort;
  std::string theFileName;
  struct sockaddr_in theCliAddr;
  struct sockaddr_in theAllowedIP;
};
}

#endif
