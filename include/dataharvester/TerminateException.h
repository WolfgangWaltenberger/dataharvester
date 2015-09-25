#ifndef __TerminateException__H
#define __TerminateException__H

#include <string>

namespace dataharvester {

class TerminateException {
public:
  TerminateException ( std::string what="" );
  std::string what() const;
private:
  std::string theWhat;
};

}

#endif
