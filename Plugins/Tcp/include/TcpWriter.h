#ifndef TcpWriter_H
#define TcpWriter_H

#include <string>
#include <map>
#include <dataharvester/AbstractWriter.h>
#include "TextWriter.h"

namespace dataharvester {
class TcpWriter : public AbstractWriter {
public:
  /**
   *  \class TcpWriter
   *  The TCP/IP backend for the Writer
   *  that enables data to be dataharvestered across the internet
   *  from multiple computers.
   *  destination syntax:
   *  dhtp://server:port/filename
   */
  TcpWriter ( const std::string & filename = "" );
  ~TcpWriter();
  void save ( const std::map < std::string, MultiType > & data,
              const std::string & name = "Tree" );
  TcpWriter & operator<< ( const Tuple & );
  void save ( const Tuple &, const std::string & prefix );
  TcpWriter * clone ( const std::string & filename ) const;
  TcpWriter * clone ( const std::string & filename ) const;
private:
  void save ( const TupleRow &, const std::string & prefix );
  std::string destination() const;
  int tcpSend( std::string ) const;
  std::string readWord() const;
  std::string readLine() const;
  bool waitForAcknowledgment() const;

private:
  std::string theRemoteFileName;
  std::string theServer;
  int thePort;
  static const int theDefaultPort = 23421;
  TextWriter theOStreamer;
};
}

#endif
