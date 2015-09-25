#ifndef __Cout__H
#define __Cout__H

#include <string>
#include <iostream>

namespace dataharvester {
namespace Cout {
  /**
   *  \class Cout
   *  A simple namespace that handles the colorcodes.
   */
  static const int Info = 1;
  static const int Warning = 2;
  static const int Error = 3;
  static const int Debug = 4;
  const char * colorCode ( int type );

  const char * error(bool force_use=false );
  const char * warning(bool force_use=false );
  const char * info(bool force_use=false );
  const char * debug(bool force_use=false );
  const char * special(bool force_use=false );

  /**
   *  (globally) turn on/off usage of colors
   */
  void useColors ( bool );

  const char * reset(bool force_use=false );

  /**
   *  newl() = reset() plus endl
   */
  const char * newl(bool force_use=false );
}
}

#endif
