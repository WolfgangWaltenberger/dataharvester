#ifndef SqliteReader_H
#define SqliteReader_H

#include <string>
#include <map>
#include <vector>
#include <dataharvester/AbstractReader.h>

namespace dataharvester {
  class DataBase;
class SqliteReader : public AbstractReader {
public:
  /**
   *  \class SqliteReader
   *  A dataharvester::Reader skeleton
   *  FIXME not yet done
   */

  SqliteReader ( const std::string & file = "" );
  SqliteReader * clone ( const std::string & file ) const;

  ~SqliteReader();

  Tuple next ();
  void reset ();

  int getNumberOfEntries() const;

  std::string filename() const;

private:
  void error();
  void getTableNames();
  void nextBlock();

private:
  std::string theFileName;
  DataBase * theDataBase;
  
  bool theHasTableNames;
  std::vector < std::string > theTableNames;
  std::vector < std::string >::const_iterator theTableIter;
  int theRow; // which row within one "block"
  int theBlockCtr; // which block
  int theRowsInBlock; // how many rows are in this block
  static const int theMaxRows=100; // how many rows in one block

};
}

#endif
