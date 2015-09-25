#ifndef RootDataCache_H
#define RootDataCache_H

#include <dataharvester/MultiType.h>
#include <string>
#include <map>
#include <vector>
#include "RootInternalNtuple.h"

class TFile;

namespace dataharvester {
class RootDataCache
{
  public:
    RootDataCache();

    ~RootDataCache();
    RootDataCache ( const RootDataCache & );
    void add ( std::string ntple, const std::map < std::string, MultiType > & mp,
               std::string ntple_description );
    void setFileName ( std::string ); // need it here because we need an empty constructor
    void write();
    void close();
    void setNeedsSave(); // define the need for "save"
    bool needsSave() const; // do we have unsaved data?
    int nTuples() const;
    void openFile();
    void describe() const;

  private:
    std::vector < RootInternalNtuple > theData;
    bool theNeedsSave;
    std::string theFileName;
    TFile * theFile;
};
}

#endif
