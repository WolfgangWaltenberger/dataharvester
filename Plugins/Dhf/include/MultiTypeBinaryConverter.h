#ifndef __MultiTypeBinaryConverter__H
#define __MultiTypeBinaryConverter__H

#include <dataharvester/MultiType.h>

namespace dataharvester {
class MultiTypeBinaryConverter {
  /** Gives unique binary representations for MultiTypes. Handles the
   * conversions, in both ways ( binary <-> MultiType )
   */
public:
  static const char * toBinary ( const MultiType & );
  static MultiType toMultiType ( const char *, MultiType::Type );
};
}

#endif
