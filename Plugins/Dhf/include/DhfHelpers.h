#ifndef DhfHelpers_H
#define DhfHelpers_H

#include <dataharvester/MultiType.h>
#include <string>

namespace dataharvester {
  class DhfHelpers {
    public:
  static const char newline;
  static const char separator;
  static const char comma;
  static const char prototype;
  static const char is_prototype;
  static const char data;

  /// Translate a char into a type.
  static MultiType::Type toType( char c );

  /// Translate a MultiType type into a char.
  static char toChar ( MultiType::Type );
  };
}

#endif
