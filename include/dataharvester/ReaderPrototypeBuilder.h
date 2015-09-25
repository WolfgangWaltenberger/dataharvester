#ifndef ReaderPrototypeBuilder_H
#define ReaderPrototypeBuilder_H

#include <string>
#include <dataharvester/ReaderManager.h>

namespace dataharvester {
template <class Reader> class ReaderPrototypeBuilder {
public:
  ReaderPrototypeBuilder < Reader > ( const std::string & filetype )
  {
    ReaderManager::self()->registerFileType (
        new Reader(), filetype );
  };
};
}

#endif
