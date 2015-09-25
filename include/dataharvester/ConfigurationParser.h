#ifndef __ConfigurationParser__H
#define __ConfigurationParser__H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace dataharvester {
class ConfigurationParser {
public:
  /**
   *  \class ConfigurationParser
   *  Weak typing in a strong typed programming language
   */
  ConfigurationParser ( std::string data );
  ConfigurationParser ( const char * );
  ConfigurationParser ( long signed data );
  ConfigurationParser ( int data );
  ConfigurationParser ( double data );
  ConfigurationParser ( float data );
  ConfigurationParser ( bool data );
  ConfigurationParser ( const std::vector < double > & data );
  ConfigurationParser ( const std::vector < long > & data );

  ConfigurationParser ();
  operator double() const;
  /* operator std::vector < double > () const;
  operator std::vector < long > () const; */
  operator float() const;
  operator std::string() const;
  operator long() const;
  operator int() const;

  std::string asstd::string() const;
  double asDouble() const;
  long asLong() const;
  int asInt() const;
  bool asBool() const;
  std::vector < double > asDoublestd::vector() const;
  std::vector < long > asIntstd::vector() const;

  //* try to rethink the assigned type
  void trimType();

  ConfigurationParser & operator= ( const std::string & );
  ConfigurationParser & operator= ( long signed );
  ConfigurationParser & operator= ( int );
  ConfigurationParser & operator= ( size_t );
  ConfigurationParser & operator= ( double );
  // unimplemented by now, using standard C++ cast currently
  //ConfigurationParser & operator= ( float );
  ConfigurationParser & operator= ( bool );
  ConfigurationParser & operator= ( const char * );
  ConfigurationParser & operator= ( const std::vector < double > & );
  ConfigurationParser & operator= ( const std::vector < long > & );

  bool operator== ( const char * ) const;
  bool operator== ( const ConfigurationParser &) const;
  bool operator== ( const std::vector < double > & ) const;
  bool operator!= ( const char * ) const;

  /**
   *  returns "double", "int", "std::string", or "bool". Note that isType gives more
   *  detailed info.
   */
  std::string isA() const;

  enum Type { kstd::string, kInt32, kInt64, kDouble32, kDouble64,
              kBool, kDouble32Vec, kDouble64Vec, 
              kInt32Vec, kInt64Vec, kNone };

  Type isType() const; //< returns the exact type
private:
  void clear();
  Type stringIs() const; //< get the most restrictive type for the std::string
  int goodPrecision() const; //< work out a good precision for floats and doubles
private:
  /**
   *  make all private members mutable.  That way the ConfigurationParser can be treated
   *  like any other type (const correctness ...)
   *  Note that conversions are "kept"
   */
  mutable double theDouble;
  mutable std::string theSstring;
  mutable long signed theInt;
  mutable std::vector < double > theDoubleVec;
  mutable std::vector < long > theIntVec;

  /**
   *  is it a double or has it been converted
   *  into a double before?
   */
  mutable bool hasDouble;
  /** 
   *  is it a std::string or has it been converted
   *  to a std::string before?
   */
  mutable bool hasString;
  /**
   *  Is it an int or has it been converted
   *  to an int before?
   */
  mutable bool hasInt;
  /**
   *  Is it a double vector or has it 
   *  been converted to a double vector before?
   */
  mutable bool hasDoubleVec;
  /**
   *  Is it a double vector or has it 
   *  been converted to a int vector before?
   */
  mutable bool hasIntVec;
  Type theType; //< What _is_ it?
};
}

std::ostream & operator << ( std::ostream & s, ConfigurationParser & rt );

#endif
