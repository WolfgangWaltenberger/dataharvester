#ifndef __HarvestingException__H
#define __HarvestingException__H

#include <string>

namespace dataharvester {
class HarvestingException {
public:
  HarvestingException ( const std::string & data="" );
  std::string what() const;
private:
  std::string theWhat;
};
}

#endif
