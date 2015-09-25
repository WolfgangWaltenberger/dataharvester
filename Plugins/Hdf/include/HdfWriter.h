#ifndef HdfWriter_H
#define HdfWriter_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/AbstractWriter.h>
#include <dataharvester/TupleRowConsistencyChecker.h>
#include <iostream>
#include <H5Cpp.h>

namespace dataharvester {
class HdfWriter : public AbstractWriter {
public:
  /**
   *  \class HdfWriter
   *  The C++ Hdf backend.
   */
  HdfWriter ( const std::string & filename = "" );
  ~HdfWriter();
  void save ( const std::map < std::string, MultiType > & data,
              const std::string & name = "Tree" );
  void save ( const Tuple & d, const std::string & prefix="" );
  HdfWriter * clone ( const std::string & filename ) const;

private:
  void createGroup ( const std::string & ); //< create a group, if necessary

  /// FIXME createCompType must include nested data
  /// create CompType, insert it into theCompTypes, return length
  int createCompType ( const std::map < std::string, MultiType > & data, const std::string & tuplename );
  int createCompType ( const TupleRow & row, const std::string & tuplename );
  // this one returns the CompType
  H5::CompType * createCompType ( const std::map < std::string, MultiType > & data, int & length );
  H5::CompType * createCompType ( const TupleRow & row, int & length );
  
  void save ( const TupleRow & row, const std::string & name, const std::string & prefix="" );
  void flatsave ( const TupleRow & row, const std::string & name, const std::string & prefix="" );
  void createDataSet ( const std::map < std::string, MultiType > & amp,
                       const std::string & tuplename, const std::string & description );

  void createDataSet ( const TupleRow & r, const std::string & tuplename,
                       const std::string & description );

  int addNestedData ( const std::map < std::string, Tuple * > & data,
                      const std::string & tuplename, int offset );
  void fillRawData ( const std::map < std::string, MultiType > & amp,
                     const std::string & tuplename );
  int fillRawData ( const TupleRow & , const std::string & tuplename, int c=0 );
  int copyRawData ( const std::map < std::string, MultiType > & amp,
                    const std::string & tuplename, int offset );
  void writeRawData ( const std::string & tuplename );
  int insertCompType ( const std::string & name, const MultiType & value,
                       const H5::CompType & tp, int offset );

  void checkExistance();
  void copyOldData ( const std::string & );

private:
  // have we already written down the tuple description?
  // std::map < std::string, bool > theHasDescription;
  // have we already written down the column description?
  std::map < std::string, std::map < std::string, bool > > theHasCDescription;
  // have we created the group already?
  std::map < std::string, bool > theHasGroup;
  std::map < std::string, H5::Group * > theGroup;
  // have we created the dataset already?
  std::map < std::string, bool > theHasDataSet;
  std::map < std::string, H5::DataSet * > theDataSet;
  std::map < std::string, H5::DataSpace * > theDataSpace;
  std::map < std::string, int > theNRows;
  std::map < std::string, H5::CompType * > theCompType;
  std::map < std::string, void * > theRawData;
  std::string theFileName;
  H5::H5File * theFile;
  TupleRowConsistencyChecker theChecker;
};
}

#endif
