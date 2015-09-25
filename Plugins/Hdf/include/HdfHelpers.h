#ifndef HdfHelpers_H
#define HdfHelpers_H

#include <dataharvester/MultiType.h>
#include <H5Cpp.h>
#include <string>

namespace dataharvester {
class HdfHelpers {
public:
  static int sizeOf ( const MultiType &, const std::string & name = "" );
  static void insertRightType ( 
    const H5::CompType * ct, std::string column, size_t offset, const MultiType & m );
};
}

#endif
