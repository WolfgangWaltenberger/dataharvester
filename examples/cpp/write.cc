#include <dataharvester/Writer.h>

int main()
{
  using namespace dataharvester;

  Tuple t("event");
  t["id"]=1; // event id=1

  t["track:id"]=1; // add first track
  t.fill("track");
  t["track:id"]=2; // add second track
  t.fill("track");

  t.fill(); // fill event

  // stream into my.txt, as well as my.hdf
  Writer::file ( "my.txt;my.hdf" ) << t;

  Writer::close(); // close all open handles
}
