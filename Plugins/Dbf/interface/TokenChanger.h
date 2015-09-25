#ifndef TokenChanger_H
#define TokenChanger_H

#include "MultiType.h"

namespace dataharvester {
class TokenChanger {
public:
  /**
   *  \class TokenChanger
   *  A class that can do simple transformations
   *  on all tokens. It can be used to e.g.
   *  put everything into lowercase letters.
   */

  TokenChanger();

  enum TokenType { kTupleName, kName, kValue, kAny };
  MultiType transform ( const MultiType & in, TokenType t=kAny );
};
}

#endif
