#include <dataharvester/HarvestingConfiguration.h>
#include "MultiTypeBinaryConverter.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>

using namespace std;
using namespace dataharvester;

namespace {
  static const int doublesize = 8;
  static const int intsize = 4;

  enum { kString='s', kInt32='i', kInt64='I', 
         kDouble64='d', kDouble32='f', kBool='b',
         kUnknown='?' } ;

  bool saveAsStrings() 
  {
    return true;
  }
}

/*
 * The MultiType binary format
 * First byte contains byte information, see the "types" enum.
 * d for 64bit double, s for string.
 * In case of a string, the second byte contains the string length.
 * then come the data, all converted to 'char's.
 */

const char * MultiTypeBinaryConverter::toBinary( const MultiType & m )
{
  if ( saveAsStrings() ) return m.asString().c_str(); // FOR development only
  ostringstream ret;
  if ( m.isA() == "string" )
  {
    ret /* << (char) kString */ << (char) m.asString().size() << m.asString();
  } else if ( m.isA() == "double" || m.isA() == "float" ) {
    double d = m.asDouble();
    int sze = sizeof(d);
    if ( sze != doublesize ) 
    {
      cout << "[MultiType] size of a double " << sze << "!=" << doublesize
           << "!!!!" << endl;
    };
    union {
      double doublebinary;
      char binarydouble[doublesize+1];
    };
    doublebinary=d;
    ret /* << (char) kDouble64 */ << binarydouble;
  } else if ( m.isA() == "int32" || m.isA() == "int64" ) {
    int i = m.asInt();
    int sze = sizeof ( i );
    if ( sze != intsize ) 
    {
      cout << "[MultiType] size of a double " << sze << "!=" << intsize
           << "!!!!" << endl;
    };
    union {
      int intbinary;
      char binaryint[intsize+1];
    };
    intbinary = i;
    ret /* << (char) kInt32 */ << binaryint;
  } else if ( m.isA() == "bool" ) {
    bool b = m.asBool();
    cout << endl << endl << "[MultiTypeBinaryConverter::toBinary] " << b << ", " << m.asInt() << endl;
    // ret << (char) kBool;
    if (b)
    {
      ret << '1';
    } else {
      ret << '0';
    }
  } else if ( m.isA() == "doublevec" || m.isA() == "floatvec" ) {
    ret << "v???"; // << m.asDoubleVector();
    // FIXME
  } else {
    ret << /* (char) kUnknown << */ m.asString();
  };
  return ret.str().c_str();
}

MultiType MultiTypeBinaryConverter::toMultiType ( const char * s, MultiType::Type type )
{
  MultiType m;
  if ( saveAsStrings () )
  {
    m=s;
    m.trimType();
    return m;
  }
  switch ( type )
  {
    case MultiType::kDouble32:
    case MultiType::kDouble64:
    {
      union {
        double doublebinary;
        char binarydouble[doublesize+1];
      };
      strncpy ( binarydouble, s, doublesize );
      m=doublebinary;
      break;
    };
    case MultiType::kInt32:
    {
      union {
        long intbinary;
        char binaryint[intsize+1];
      };
      strncpy ( binaryint, s, intsize );
      m=intbinary;
      break;
    };
    case MultiType::kBool:
    {
      bool b = false;
      if ( s=="1" ) b=true;
      m=b;
      break;
    };
    case MultiType::kString:
    case MultiType::kNone:
    default:
    {
      m=s; break;
    };
  };
  
  // m.defineAs ( type );
  /* cout << "[MultiTypeBinaryConverter] m=" << m.asString()
       << " isA " << m.isA() << endl; */
  return m;
}
