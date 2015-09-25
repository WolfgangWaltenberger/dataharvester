#include "HarvestingConfiguration.h"
#include "HarvestingException.h"
#include "HarvestingHelpers.h"
#include "HarvestingCout.h"
#include "ConfigurationSetter.h"
#include "DataHarvester.h"
#include <unistd.h>
#include <cstdlib>

int main( int argc, char * argv[] )
{
  Tuple data;
  data["blue"]=(float) 0;
  DataHarvester::file( "--" ).save ( data, "yo" );
  DataHarvester::close();
};
