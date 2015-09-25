#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Writer.h>
#include <dataharvester/Reader.h>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/TupleNameFilter.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/Cout.h>
#include <dataharvester/SimpleConfiguration.h>
#include <dataharvester/StringTools.h>
#include <dataharvester/UniversalManipulator.h>
#include <dataharvester/ConfigurationSetter.h>
#include "config.h"
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <iomanip>

using namespace dataharvester;
using namespace std;

namespace {
  void usage( char * name )
  {
    cout << "Usage: " << name << " [-h] [-V] [-l] [-o<name1=value1,name2=value2>] [-n<num>] "
         << "[-v<num>] [-f<filter>] [-c] [-b] [-p<n>] <src1> <src2> ... <srcN> "
         << "<dest>" << endl
         << endl
         // << "options:" << endl
         << "        -h, --help          Show this help." << endl
         << "        -V, --version       Print version number and exit." << endl
         << "        -l, --list          List all SimpleConfigurable options" << endl
         << "                            (supply files to trigger loading the appropriate plugins)" << endl
         << "        -v, --verbose NUM   Set the verbosity." << endl
         << "        -f, --filter <str>  Define a filter (works on the tuplenames)." << endl
         << "        -d, --dropcolumn <str>  Name columns to drop (comma-separated list)." << endl
         << "        -o, --set           Set SimpleConfigurables." << endl
         << "        -p, --progress NTH  Show progress (print a dot every nth tuple)." << endl
         << "        -b, --bar           Show progress bar." << endl
         << "        -n, --num NUM       Maximum number of tuples to be read." << endl
         << "        -c, --color         Enable color mode." << endl;
  }

  void version( char * name )
  {
    cout << " " << PACKAGE_STRING << endl;
  }

  string removeOption ( string text, const string & option, const string & longoption="" )                      
  {                                                                                                             
    string::size_type pos = text.find ( longoption );                                                           
    string::size_type sz = longoption.size();                                                                   
    if ( pos != string::npos )                                                                                  
    {                                                                                                           
      text=text.replace ( pos, sz, "" );                                                                        
      if ( text[0] == '=' )                                                                                     
      {                                                                                                         
        text=text.replace(0,1,"");                                                                              
      }                                                                                                         
    }                                                                                                           
    pos = text.find ( option );                                                                                 
    sz = option.size();                                                                                         
    if ( pos != string::npos )                                                                                  
    {                                                                                                           
      text=text.replace ( pos, sz, "" );                                                                        
    }                                                                                                           
    return text;                                                                                                
  }               
}

int main( int argc, char * argv[] )
{
  if ( argc < 2 )
  {
    usage ( argv[0] );
    return 0;
  };
        
  vector < string > sources;
  string dest = argv[argc-1];

  int first_source = 1;
  int max_lines = -1; // maximum number of tuple rows that are read
  int show_every=0;
  int every_nth=0;
  bool list_configurables=false;
  bool show_bar=false;
  string filter="";
  string dropcolumn="";

  int max_arg=argc-2 < 2 ? 2 : argc-2;
  for ( int i=first_source; i< max_arg ; i++ )
  {
    if ( strncmp ( argv[i], "-v", 2 ) == 0 || strncmp ( argv[i], "--verbose", 9 ) == 0  )
    {
      string v=removeOption ( argv[i],"-v", "--verbose" ); 
      int verbosity = atoi ( v.c_str() );
      Helpers::setVerbosity ( verbosity );
      first_source++;
    } else if ( strncmp ( argv[i], "-h", 2 ) == 0 || strncmp ( argv[i], "--help", 6 ) == 0 )
    {
      usage( argv[0] );
      exit(0);
    } else if ( strncmp ( argv[i], "-b", 2 ) == 0 || strncmp ( argv[i], "--bar", 5 ) == 0 )
    {
      show_bar=true;
      first_source++;
    } else if ( strncmp ( argv[i], "-p", 2 ) == 0 || strncmp ( argv[i], "--progress", 10 ) == 0 )
    {
      string v=removeOption ( argv[i],"-p", "--progress" ); 
      first_source++;
      if ( v=="" )
      {
        i++; first_source++;
        v=argv[i];
      }
      show_every = atoi ( v.c_str() );
    } else if ( strncmp ( argv[i], "-f", 2 ) == 0 || strncmp ( argv[i], "--filter", 8 ) == 0 )
    {
      filter=removeOption ( argv[i],"-f", "--filter" ); 
      first_source++;
      if ( filter=="" )
      {
        i++; first_source++;
        filter=argv[i];
      }
    } else if ( strncmp ( argv[i], "-c", 2 ) == 0 || strncmp ( argv[i], "--dropcolumn", 12 ) == 0 )
    {
      filter=removeOption ( argv[i],"-c", "--dropcolumn" ); 
      first_source++;
      if ( dropcolumn=="" )
      {
        i++; first_source++;
        dropcolumn=argv[i];
      }
    } else if ( strncmp ( argv[i], "-l", 2 ) == 0 || strncmp ( argv[i], "--list", 6 ) == 0 )
    {
      list_configurables=true;
      first_source++;
    } else if ( strncmp ( argv[i], "-o", 2 ) == 0 || strncmp ( argv[i], "--set", 5 ) == 0  )
    {
      string v=removeOption ( argv[i],"-o", "--set" ); 
      if ( v=="" )
      {
        i++; first_source++;
        v=argv[i];
      }
      ConfigurationSetter::set ( v );
      first_source++;
    } else if ( strncmp ( argv[i], "-n", 2 ) == 0 || strncmp ( argv[i], "--num", 5 ) ==0 )
    {
      string v=removeOption ( argv[i],"-n", "--num" ); 
      if ( v=="" )
      {
        i++; first_source++;
        v=argv[i];
      }
      max_lines = atoi ( v.c_str() );
      if ( max_lines < 0 )
      {
        cout << "[converter] Error: a negative maximum number of tuples given (n="
             << max_lines << "). Will ignore sign." << endl;
        max_lines = abs ( max_lines );
      }

      first_source++;
    } else if ( strncmp ( argv[i], "-V", 2 ) == 0 || strncmp ( argv[i], "--version", 9 ) ==0 )
    {
      version( argv[0] );
      exit(0);
    } else if ( strncmp ( argv[i], "-c", 2 ) == 0 || strncmp ( argv[i], "--color", 7 ) ==0  )
    {
      Cout::useColors ( true );
      first_source++;
   /* } else {
      cout << Cout::error() <<
        "[converter] error: do not understand argument ``"
        << argv[i] << "''. Dropping." << endl;
      first_source++; */
    };
  };

  if ( Helpers::verbose() > 0 )
  {
    cout << Cout::info()
         << "[converter] Converting ";
  };

  for ( int i=first_source; i< argc-1 ; i++ )
  {
    sources.push_back ( argv[i] );
    if ( Helpers::verbose() > 0 )
    {
      cout << argv[i] << " ";
      if ( i < (argc-2) ) cout << "+ ";
    };
  };
  if ( Helpers::verbose() > 0 )
    cout << " -> " << argv[argc-1] << Cout::reset() 
         <<endl;
  string tuplename="";
  map < string, MultiType > mp;

  int n_line=0;
  if ( max_lines > 0 && Helpers::verbose() )
  {
    cout << Cout::debug() 
         << "[converter] max lines=" << max_lines 
         << Cout::reset() << endl;
  };
  int n_entries=0; // -1 we do not know how many tuple rows to read in

  if ( show_bar )
  {
    for ( vector< string >::const_iterator source=sources.begin();
          source!=sources.end() ; ++source )
    {
      try {
        AbstractReader & file = Reader::file ( *source ); 
        if ( filter != "" )
        {
          TupleNameFilter tnfilter ( filter );
          file.setFilter ( tnfilter );
        }
        int tmp = file.getNumberOfEntries();
        if ( tmp == -1 )
        {
          n_entries = -1;
          break;
        };
        n_entries+=tmp;
      } catch (...) {
        break;
      };
    };
  };

  if ( n_entries > max_lines && max_lines > 0 ) n_entries = max_lines;
  if ( Helpers::verbose() > 0 )
  {
    if ( n_entries < 0 )
    {
      cout << "[converter] cannot anticipate number of tuples."
           << endl;
    } else {
      cout << "[converter] will read " << n_entries << " tuples."
           << endl;
    };
  };

  for ( vector< string >::const_iterator source=sources.begin();
        source!=sources.end() ; ++source )
  {
    try {
      int last_percentage = -1;
      AbstractReader & file = Reader::file ( *source ); 
      if ( filter !="" )
      {
        TupleNameFilter tnfilter ( filter );
        file.setFilter ( tnfilter);
      }
      UniversalManipulator manipulator;
      if ( dropcolumn!= "" )
      {
        list < string > columns = StringTools::split ( dropcolumn, "," );
        for ( list < string >::const_iterator i=columns.begin(); 
              i!=columns.end() ; ++i )
          manipulator.dropColumn ( *i );
      }
      try {
        while ( true )
        {
          Tuple tuple =file.next();
          // cout << "[converter] before drop: " << tuple.dump() << endl;
          manipulator.manipulate ( tuple );
          // cout << "[converter] after drop: " << tuple.dump() << endl;
          // cout << "[converter] dumping " << endl;
          // tuple.dump();
          if ( max_lines > 0 && n_line >= max_lines )
          {
            // cout << "[converter] max_lines exceeded" << endl;
            break;
          };
          if ( show_every > 0 && fmod ( (float) n_line, (float) show_every ) == 0 )
          {
            every_nth++;
            if ( fmod ( (float) every_nth, 100 ) == 0 )
            {
              cout << (int) every_nth / 100 << flush;
            } else {
              cout << "." << flush;
            };
          };
          n_line++;
          Writer::file ( dest ) << tuple;
          if ( show_bar && n_entries < 0 )
          {
            cout << "[converter] dont know number of entries - cannot draw bar" << endl;
          };
          if ( show_bar && n_entries > 0 )
          {
            int percentage = int ( (float) n_line / (float) n_entries * 1000. );
            if ( percentage > last_percentage )
            {
              cout << "Progress [" << setw(3) << setprecision(1) << setiosflags ( ios::fixed )
                   << percentage / 10. << "%]" << endl;
              cout << "[A";
              last_percentage=percentage;
            };
          };
        };
      } catch ( TerminateException & e ) {
        if ( e.what().size() ) {
          cout << "[converter] terminating: " << e.what() << endl;
        }
      };
    } catch ( HarvestingException & e ) {
      cout << "[converter] Skipping  " << e.what()
           << endl;
      cout << "[converter]  (skipping)" << endl;
    } catch ( std::exception & e ) {
      cout << "[converter] Exception " << e.what() << " caught in " << *source
           << ": " << e.what() << endl;
    } catch ( ... ) {
      cout << "[converter] Exception caught in " << *source
           << endl;
    };
  };
  Writer::close();
  if ( list_configurables )
  {
    cout << "List of all SimpleConfigurables:" << endl;
    cout << "================================" << endl;
    SimpleConfiguration::current()->status();
  };
}

