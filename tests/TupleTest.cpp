#include "HarvestingConfiguration.h"
#ifdef WITH_SEEDER
#ifdef WITH_HARVESTER
#include "Tuple.h"
#include "HarvestingHelpers.h"
#include "HarvestingCout.h"
#include "DataHarvester.h"

void testTuple()
{
  Tuple t("event");
  t.describe("Event Tuple. Stores the ... ahem .. events");
  t["type"]="type a event a:3==5";
  t["id"]=5;
  t["t"]=3.0;
  t["points:x"]=1.0;
  t.describe("Points in 3D space","points");
  t["points:y"]=1.0;
  t["points:z"]=1.0;
  t.fill("points");
  t["vtx:x"]=3.;
  t["vtx:asstrk:id"]=3;
  t["vtx:asstrk:x"]=3.2;
  t.fill("vtx:asstrk");
  t["vtx:asstrk:id"]=4;
  t["vtx:asstrk:x"]=4.2;
  t.fill("vtx:asstrk");
  t.fill("vtx");
  t.fill();
  DataHarvester::file("me.txt") << t;
}

int main( int argc, char * argv[] )
{
  testTuple();
}

#endif /* def HARVESTER */
#endif /* def SEEDER */
