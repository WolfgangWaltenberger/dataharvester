#include "HarvestingConfiguration.h"
#include "HarvestingCout.h"
#include "DataHarvester.h"
#include "SystemHarvester.h"
#include <cassert>

namespace {
  using namespace HarvestingCout;

  void ok()
  {
    cout << " ... " << info() << "ok" << reset() << endl;
  };
};
int main()
{
  SystemHarvester( "stdout" ).save();
  DataHarvester::close();
};
