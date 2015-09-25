#ifndef RootInternalNtuple_H
#define RootInternalNtuple_H

#include <dataharvester/MultiType.h>
#include <string>
#include <vector>
#include <map>
#include <dataharvester/TupleRowConsistencyChecker.h>
class TTree;

namespace dataharvester {
class RootInternalNtuple
{
public:
  RootInternalNtuple ( const std::string & name, 
      const std::map < std::string, MultiType > & mp,
                       bool firstwrite=true );
  ~RootInternalNtuple ();

  void add ( const std::map < std::string, MultiType > & mp );
  std::string name() const;
  void write();
  int nRows() const; //< return the number of rows ( theData.size() )
  void describe() const;

private:
  void initWrite(); //< the first time writing, we need to initialize things

  TupleRowConsistencyChecker theChecker;
  std::string theName;
  bool firstWrite;
  std::map < std::string, MultiType > theHeader;
  std::vector < std::vector < MultiType > > theData;
  std::map < std::string, double > theDoubles;
  std::map < std::string, int > theInts;
  std::map < std::string, std::vector < long int > * > theIntVecs;
  std::map < std::string, std::vector < double > * > theDoubleVecs;
  std::map < std::string, char * > theStrings;
  std::map < std::string, unsigned int > theStringsSize;
  TTree * theTree;
};
}

#endif
