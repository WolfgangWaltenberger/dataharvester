#ifndef WriterPrototypeBuilder_H
#define WriterPrototypeBuilder_H

#include <string>
#include <dataharvester/WriterManager.h>

namespace dataharvester {
template < class Harvester, int type = WriterManager::FileFormat > 
    class WriterPrototypeBuilder {
public:
  WriterPrototypeBuilder < Harvester, type > (
      std::string filetype_or_protocol )
  {
    WriterManager::self()->registers ( type,
        new Harvester(), filetype_or_protocol );
  };
};
}

#endif
