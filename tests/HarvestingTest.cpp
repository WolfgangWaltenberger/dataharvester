#include "HarvestingConfiguration.h"
#include "HarvestingException.h"
#include "Helpers.h"
#include "Cout.h"
#include "ConfigurationSetter.h"
#include "Writer.h"
#include <unistd.h>
#include <cstdlib>
#include <math.h>

using namespace harvest;

namespace {
  string name ( int i )
  {
    switch ((int) (fmod((double) i,4.)) )
    {
      case 0:
        return "Duck, Donald";
      case 1:
        return "Winni";
      case 2:
        return "Daisy";
      case 3:
        return "Anonymous";
      default:
        return "NN";
    };
    return "NN";
  }

  vector < string > destinations()
  {
    // list of all data sinks
    vector < string > dests;

    dests.push_back ( "stdout" );
    /*
    dests.push_back ( "file:/tmp/test.txt" );
#ifdef WITH_ROOT
    dests.push_back ( "test.root" );
#endif
    */
#ifdef WITH_TCP
    dests.push_back ( "dhtp://localhost:23142/tmp/test.txt" );
#ifdef WITH_ROOT
    dests.push_back ( "dhtp://localhost:23142/tmp/test2.root" );
#endif
#endif

#ifdef WITH_ZLIB
    dests.push_back ( "test.txt.gz" );
#endif

#ifdef WITH_PICKLE
    dests.push_back ( "test.p" );
#endif
    return dests;
  }

  void usage( string name )
  {
    cout << "Usage: " << name << " -h -c -n <number of rows> -d <dest> -v <level> [-o<name1=value1,name2=value2>]" << endl;
    cout << endl;
    cout << "  -h         Help" << endl
         << "  -o         Set SimpleConfigurables." << endl;
    cout << "  -c         Enable color mode" << endl;
    cout << "  -d <dest>  Destination string (default depends on configuration)" << endl;
    cout << "  -n <num>   Number of data rows that are produced (default: 5)" << endl;
    cout << "  -v <level> Verbosity (default 1)" << endl;
  }
}

int main( int argc, char * argv[] )
{
  int n_rows=5;
  bool colors = false;
  int verbosity=1;
  string dest="";
  while (1)
  {
    int ret = getopt ( argc, argv, "hcn:v:d:o:" );
    if ( ret == -1 ) break;
    switch (ret)
    {
      case 'h': { usage(argv[0]); exit(0); };
      case 'c': { colors=true; break; };
      case 'd': { dest=optarg; break; };
      case 'n': { n_rows = atoi ( optarg ); break; };
      case 'o': { ConfigurationSetter::set ( optarg ); break; };
      case 'v': { verbosity = atoi ( optarg ); break; };
      default: { cout << "[HarvestingTest] unknown option " << (char) ret << endl; };
    };
  };
  Cout::useColors ( colors );
  Helpers::setVerbosity( verbosity );

  cout << endl << Cout::info()
       << "[HarvestingTest] writing " << n_rows << " rows." << endl;
  vector < string > dests;
  if ( dest.size() )
  {
    dests.push_back ( dest );
  } else {
    dests=destinations();
  };

  cout << "[HarvestingTest] data sinks are: ";
  for ( vector< string >::const_iterator i=dests.begin(); 
        i!=dests.end() ; ++i )
  {
    if ( i!=dests.begin() ) cout << ", ";
    cout << *i;
  };
  cout << endl << endl;
  cout << "[HarvestingTest] ------------------------------------------------" << endl;
  cout << Cout::reset();
  for ( int i=1; i<= n_rows ; i++ )
  {
    map < string, MultiType > data;
    data["Name (The name of the person)"]= name(i);
    data["Age (The person's age)"] = 100. * drand48();
    vector < double > scores;
    scores.push_back ( 3. );
    scores.push_back ( 3.5 );
    scores.push_back ( 5.2 );
    // data["Persons's scores"] = scores;
    data["married (Is the person married?)"] = false;
    data["row (Ntuple row)"]=i*11;
    data["gauss1"]=0.75;
    data["gauss2"]=0.22;

    for ( vector< string>::const_iterator i=dests.begin(); i!=dests.end() ; ++i )
    {
      try {
        Writer::file(*i).save ( data, 
            "People (An ntuple that lists all people)" );
        data["gauss1"]=0.75;
        data["gauss2"]=0.22;
        data["married (Is the person married?)"] = true;
        Writer::file(*i).save ( data, 
            "Gaussians" );
      } catch ( HarvestingException & e ) {
        cout << "[HarvestingTest] Caught: ``" << e.what() << "''" << endl;
      };
    };
  };
  cout << Cout::info();
  cout << "[HarvestingTest] ------------------------------------------------" << endl;
  cout << Cout::reset() << endl;
  Writer::close();
}
