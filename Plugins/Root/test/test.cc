#include <dataharvester/Writer.h>

int main () 
{
  using namespace dataharvester;
  using namespace std;
  dataharvester::Tuple t("t");
  for ( int i=0; i< 10 ; i++ )
  {
    t["id"]=i;
    vector < long > longs;
    longs.push_back ( i );
    longs.push_back ( 2*i );
    t["longs"]=longs;
    vector < double > doubles;
    doubles.push_back ( float (i ) / 2. );
    doubles.push_back ( float (i ) / 3. );
    if ( i > 5 )
      doubles.push_back ( float (i ) / 4. );
    t["doubles"]=doubles;
    dataharvester::Writer::file("test.root;test.txt") << t;
  }

  dataharvester::Writer::close();
}
