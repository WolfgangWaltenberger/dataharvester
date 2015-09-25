#include "HarvestingConfiguration.h"
#ifdef WITH_HARVESTER
#include "GenericDataCache.h"
#include "SimpleConfigurable.h"

using namespace std;
using namespace harvest;

namespace {
  int max_conf_rows ()
  {
    int ret = SimpleConfigurable<int> (100000,"GenericDataCache:MaxRows" ).value();
    return ret;
  }
}

GenericDataCache::GenericDataCache( int max_rows ) : theMaxRows ( max_rows )
{
  if ( max_rows < 0 )
  {
    theMaxRows=max_conf_rows();
  };
}

GenericDataCache::~GenericDataCache()
{
  if ( theData.size() )
  {
    cout << "[GenericDataCache] Warning!! Destroying non-empty GenericDataCache. Size="
         << theData.size() << endl;
  };
}

bool GenericDataCache::hasSpace() const
{
  // FIXME what about theMaxRows=-1?
  return ( theData.size() < (unsigned int) theMaxRows );
}

bool GenericDataCache::push ( string ntple, const map < string, MultiType > & mp )
{
  pair < string, map < string, MultiType > > tmp ( ntple, mp );
  theData.push ( tmp );
  return hasSpace();
}

bool GenericDataCache::pop ( string & ntple, map < string, MultiType > & data )
{
  pair < string, map < string, MultiType > > tmp = theData.front();
  theData.pop();
  return !(theData.empty());
}

#endif /* def WITH_HARVESTER */
