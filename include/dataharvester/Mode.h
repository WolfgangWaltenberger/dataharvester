#ifndef Mode_H
#define Mode_H

#include <string>

namespace dataharvester {
class Mode {
public:
  enum FileMode { Update=0, Recreate=1 };
  static void setFileMode ( FileMode m = Recreate );
  static FileMode fileMode();
  static std::string fileModeString();
};
}

#endif
