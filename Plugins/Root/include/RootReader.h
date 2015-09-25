#ifndef RootReader_H
#define RootReader_H

#include <string>
#include <map>
#include <dataharvester/AbstractReader.h>

class TFile;
class TList;
class TTree;
class TBranch;

namespace dataharvester {
class RootReader : public AbstractReader {
public:
  /**
   *  \class RootReader
   *  Backend that reads from ROOT files.
   */

  RootReader ( const std::string & file="" );
  RootReader ( TFile * );
  RootReader ( const RootReader & o );
  RootReader * clone ( const std::string & file ) const;
  ~RootReader();

  void reset();
  Tuple next ( );
  int getNumberOfEntries() const;
  std::string filename() const;

  TFile * file();

private:
  void addBranch ( Tuple & t, TBranch * branch, const std::string & prefix="", int evt=-1 );
  void addEntries ( Tuple & t, TBranch * branch, const std::string & prefix="" ) const;
  void addCurrentTree ( Tuple & t );
  void addIndexedTree ( Tuple & t );
  bool getNewKey() const;

private:
  std::string theFileName;
  TFile * theFile;
  TList * theList;
  mutable int theTupleCounter;
  mutable int theKeyCounter;
  mutable int theSubDirKeyCounter;
  mutable int theEventCounter;
  mutable std::map < std::string, int > theOffsets;
};
}

#endif
