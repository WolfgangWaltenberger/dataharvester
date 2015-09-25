#ifndef TcpDataServerThread_H
#define TcpDataServerThread_H

#include <string>
#include <iostream>
#include <arpa/inet.h>

namespace dataharvester {
class TcpDataServerThread {
public:
  /**
   *  \class TcpDataServerThread
   *  Implements one "data channel"
   */

  TcpDataServerThread ( int skt, std::string filename, unsigned short port,
                        const sockaddr_in & client, int * count ); 
  ~TcpDataServerThread();

  void communicate();
private:
  void sendHello();
  void interpret ( const std::string & line );
private:
  int theSocket;
  int thePort;
  std::string theFileName;
  struct sockaddr_in theCliAddr;
  int * theCounter;
};
}

#endif
