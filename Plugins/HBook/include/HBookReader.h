#ifndef HBookReader_H
#define HBookReader_H

#include <string>
#include <map>
#include <dataharvester/AbstractReader.h>

class THbookFile;
class TList;
class TTree;
class THbookTree;

namespace dataharvester {
class HBookReader : public AbstractReader {
public:
  /**
   *  \class HBookReader
   *  A class that is meant to perform the "inverse operation" of the
   *  RootDataHarvester. That is, from a RootDataHarveste'd .txt file, it
   *  creates the STL std::maps.
   */

  HBookReader ( const std::string & file="" );
  HBookReader ( THbookFile * );
  HBookReader * clone ( const std::string & file ) const;
  ~HBookReader();

  /**
   *  Currently you can iterate thru by
   *  while ( nextNtuple(...) ) { ... };
   */
  bool nextNtuple ( std::string &, std::map < std::string, MultiType > & );
  void reset();
  Tuple next ( );
  int getNumberOfEntries() const;
  std::string filename() const;

  THbookFile * file();

private:
  bool getNewKey() const;

private:
  std::string theFileName;
  THbookFile * theFile;
  TList * theList;
  mutable TTree * theCurTree;
  mutable THbookTree * theCurHTree;
  mutable int theTupleCounter;
  mutable int theKeyCounter;
};
}

#endif
