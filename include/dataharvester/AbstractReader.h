#ifndef AbstractReader_H
#define AbstractReader_H

#include <dataharvester/MultiType.h>
#include <dataharvester/Tuple.h>
#include <dataharvester/Filter.h>
#include <dataharvester/Manipulator.h>
#include <string>
#include <iostream>
#include <map>

namespace dataharvester {
class AbstractReader {
public:
  /**
   *  \class AbstractReader
   *  Purely abstract base class for all data seeders.
   */
  AbstractReader();
  AbstractReader( const AbstractReader & o );


  virtual ~AbstractReader();

  /**
   *  Return next Tuple
   */
  virtual Tuple next ( ) = 0;

  /**
   *  back to first Tuple
   */
  virtual void reset() = 0;

  virtual AbstractReader * clone ( const std::string & filename ) const = 0;

  virtual std::string filename() const = 0;

  /** Set a Filter. Same procedure for all Readers. */
  void setFilter ( const Filter & );

  /** Set a Manipulator. Same procedure for all Readers. */
  void setManipulator ( const Manipulator & );

  /**
   *  Some seeders know how many tuple rows to expect.
   *  A client might ask.
   *  -1 means 'no information'
   */
  virtual int getNumberOfEntries() const = 0;

  /// Describe the reader.
  std::string describe() const;

protected:
  /**
   *  Should be called by the backend after delivering a Tuple.
   *  Takes care of the filter _and_ the manipulator.
   *  First the manipulator is called into action,
   *  then the filter is called.
   */
  Tuple postProcess ( Tuple & t );

protected:
  Filter * theFilter;
  Manipulator * theManipulator;

};
}

std::ostream & operator << ( std::ostream & s, dataharvester::AbstractReader & );

#endif
