#ifndef SqliteWriter_H
#define SqliteWriter_H

#include <string>
#include <map>
#include <dataharvester/AbstractWriter.h>
#include <sqlite3.h>

namespace dataharvester {
class SqliteWriter : public AbstractWriter {
public:
  /**
   *  \class SqliteWriter
   *  A skeleton for a writer
   */
  SqliteWriter ( const std::string & filename = "" );
  ~SqliteWriter();

  SqliteWriter * clone ( const std::string & filename ) const;

  void save ( const Tuple & d, const std::string & prefix="" );

private:
  void save ( const TupleRow &, const std::string & tuple );
  void exec ( const std::string & );
  void error();

private:
  std::string theFileName;
  sqlite3 * theDataBase;
  char ** theErr;
  std::map < std::string, bool > theHasTable;
};
}

#endif
