#ifndef HdfReader_H
#define HdfReader_H

#include <string>
#include <map>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/MultiType.h>
#include "H5Cpp.h"

namespace dataharvester {
class HdfReader : public AbstractReader {
public:
  /**
   *  \class HdfReader
   *  not operational
   */

  HdfReader ( const std::string & file = "" );
  HdfReader * clone ( const std::string & file ) const;
  HdfReader ( const HdfReader & o );
  ~HdfReader();

  /**
   *  Currently you can iterate thru by
   *  while ( nextNtuple(...) ) { ... };
   */
  bool nextNtuple ( std::string &, std::map < std::string, MultiType > & );
  Tuple next ( );
  void reset ();
  int getNumberOfEntries() const;
  bool readRow( std::map < std::string, MultiType > & );
  std::string filename() const;

private:
  MultiType getMultiType ( const H5::CompType & ctype, int idx, int row_offset );
  std::map < std::string, MultiType > retrieveRow ( int r );

private:
  std::string theFileName;
  std::string theCurrentGroup;
  int theCurrentGroupIdx; // the number of the currently active group
  int theNRows; // the number of rows within a group
  int theCurrentRow; // the current row within a group
  int theAllocCounter; // counts allocations / deallocations
  signed int theMaxGroupNr; // the number of groups in the file
  bool openFile(); // opens the file
  bool getNewGroup(); // open the next group
  H5::H5File * theFile;
  H5::DataSet theDataSet; // current dataset
  void * theRaw; // container for the raw data
};
}

#endif
