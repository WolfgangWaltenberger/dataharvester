#ifndef OdbcWriter_H
#define OdbcWriter_H

#include <string>
#include <map>
#include <dataharvester/AbstractWriter.h>

namespace dataharvester {
class OdbcWriter : public AbstractWriter {
public:
  /**
   *  \class OdbcWriter
   *  A skeleton for a writer
   */
  OdbcWriter ( const std::string & filename = "" );
  ~OdbcWriter();

  OdbcWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );
  void openConnection();

private:
  std::string theFileName;
};
}

#endif
