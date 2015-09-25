#ifndef GenericDataCache_H
#define GenericDataCache_H

#include "MultiType.h"
#include <string>
#include <queue>
#include <map>
#include <utility>

class GenericDataCache
{
    /**
     *  GenericDataCache implements a general-purpose
     *  queue for data rows.
     */
  public:
    /* \param max_rows defines how many rows can be stored
     * before the data cache "complains" (rows are then still stored,
     * as long as the machine can). max_rows=-1 delegates
     * the definition to the SimpleConfigurable
     * GenericDataCache:MaxRows=100000.
     */
    GenericDataCache( int max_rows=-1 );
    ~GenericDataCache();

    /**
     *  still space in the cache?
     */
    bool hasSpace() const;

    /**
     * push the data.
     * return true as long as we're below the maximum capacity.
     */
    bool push ( std::string ntple, const std::map < std::string, MultiType > & mp );

    /**
     *  pop the data. returns true as long as there
     *  is still more to be retrieved.
     */
    bool pop ( std::string & ntple, std::map < std::string, MultiType > & data );
  private:
    int theMaxRows;
    std::queue < std::pair < std::string, std::map < std::string, MultiType > > > theData;
};

#endif
