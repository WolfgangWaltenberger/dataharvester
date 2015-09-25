#ifndef TcpCommServer_H
#define TcpCommServer_H

#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include "TcpDataServer.h"
#include <arpa/inet.h>

namespace dataharvester {
class TcpCommServer {
public:
  /**
   *  \class TcpCommServer
   *  The server class of the communication channel.
   *  This class serves the basic functionality
   *  of the dataharvesteringd daemon.
   *  It creates TcpDataServers, whenever theyre
   *  requested.
   */

  TcpCommServer ( unsigned short port = 23142 ); 
  ~TcpCommServer();
  void destroy(); // close down comm line
  static std::vector < std::string > commands(); // returns list of commands

private:
  void initCleanup();
private:
  int theOrigSocket;
  std::ostream * theStream; // messages
  bool theDelStream; // do we need to delete theStream?
  bool theIsDestroyed; // comm line closed already?
};
}

#endif
