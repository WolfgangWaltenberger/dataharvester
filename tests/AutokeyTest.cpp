#include "HarvestingConfiguration.h"
#include "HarvestingHelpers.h"
#include "HarvestingException.h"
#include "HarvestingCout.h"
#include "DataHarvester.h"
#include <unistd.h>
#include <cstdlib>
#ifdef WITH_ROOT
#include "TRandom.h"
#endif

using namespace harvest;

namespace {
  string 
    name ( int i )
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
  };

    /*
  vector < string > destinations()
  {
    // list of all data sinks
    vector < string > dests;

    dests.push_back ( "stdout" );
    dests.push_back ( "file:/tmp/test.txt" );
#ifdef WITH_ROOT
    dests.push_back ( "test.root" );
#endif

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
  };
    */

  void usage( string name )
  {
    cout << "Usage: " << name << " -h -c -n <number of rows> -d <dest> -v <level>" << endl;
    cout << endl;
    cout << "  -h         Help" << endl;
    cout << "  -c         Enable color mode" << endl;
    cout << "  -d <dest>  Destination string (default depends on configuration)" << endl;
    cout << "  -n <num>   Number of data rows that are produced (default: 5)" << endl;
    cout << "  -v <level> Verbosity (default 1)" << endl;
  };
};


int 
main( int argc, char * argv[] )
{
  int n_rows=15;
  bool colors = false;
  int verbosity=1;
  string dest="";
  while (1)
  {
    int ret = getopt ( argc, argv, "hcn:v:d:" );
    if ( ret == -1 ) break;
    switch (ret)
    {
      case 'h': { usage(argv[0]); exit(0); };
      case 'c': { colors=true; break; };
      case 'd': { dest=optarg; break; };
      case 'n': { n_rows = atoi ( optarg ); break; };
      case 'v': { verbosity = atoi ( optarg ); break; };
      default: { cout << "[harvestingd] unknown option " << (char) ret << endl; };
    };
  };
  Cout::useColors ( colors );
  Helpers::setVerbosity( verbosity );


  cout << endl << Cout::info()
       << "[AutokeyTest] writing " << n_rows << " rows." << endl;
/*
  vector < string > dests;
  if ( dest.size() )
  {
    dests.push_back ( dest );
  } else {
    dests=destinations();
  };

  cout << "[AutokeyTest] data sinks are: ";
  for ( vector< string >::const_iterator i=dests.begin(); 
        i!=dests.end() ; ++i )
  {
    if ( i!=dests.begin() ) cout << ", ";
    cout << *i;
  };
  cout << endl << endl;
  */
  cout << "[AutokeyTest] ------------------------------------------------" << endl;

  //DataHarvester::file("test.txt").setOptions( "People", "AUTOKEY" );

  cout << Cout::reset();
//  #ifdef WITH_ROOT
  TRandom rn;
//  #endif
  for ( int i=1; i<= n_rows ; i++ )
  {
    Tuple data;
    data["Name (The name of the person)"]= name(i);
    data["Age (The person's age)"] = 100. * drand48();

    vector < double > scores;
    scores.push_back ( 3. );
    scores.push_back ( 3.5 );
    scores.push_back ( 5.2 );
    // data["Persons's scores"] = scores;

    data["married (Is the person married?)"] = false;
    data["row (Ntuple row)"]=i;
//#ifdef WITH_ROOT
    data["gauss1"]=(double) rn.Gaus();
    data["gauss2"]=(double) rn.Gaus(1.0,2.0);
//#endif
    cout << "generated tuple " << i << endl;

    DataHarvester::file("test.txt").save ( data, 
        "People (An ntuple that lists all people)" 
        ,"AUTOKEY" 
        );
    cout << "wrote tuple " << i << endl;

    /*
    for ( vector< string>::const_iterator i=dests.begin(); 
        i!=dests.end() ; ++i )
    {
      try {
        DataHarvester::file(*i).save ( data, 
            "People (An ntuple that lists all people)", "AUTOKEY" );
        #ifdef WITH_ROOT
        data["gauss1"]=(double) rn.Gaus();
        data["gauss2"]=(double) rn.Gaus(1.0,2.0);
        data["married (Is the person married?)"] = true;
        DataHarvester::file(*i).save ( data, 
            "Gaussians", "AUTOKEY" );
        #endif
      } 
      catch ( HarvestingException & e ) 
      {
        cout << "[AutokeyTest] Caught: ``" << e.what() << "''" << endl;
      };
    };
    */
  };
  DataHarvester::close();
  cout << Cout::info();
  cout << "[AutokeyTest] ------------------------------------------------" 
    << endl;
  cout << Cout::reset() << endl;
};
