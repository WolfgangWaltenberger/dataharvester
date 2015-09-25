#include <dataharvester/HarvestingConfiguration.h>
#include "XmlReader.h"
#include <dataharvester/TerminateException.h>
#include <dataharvester/CommentsCollector.h>
#include <string>

#include <libxml++/libxml++.h>

using namespace dataharvester;
using namespace std;

namespace {
  int verbose()
  {
    return 0;
  }

  string createSub ( map < int, string > & h, unsigned d )
  {
    // unsigned max = h.size() < d ? h.size() : d;
    string ret="";
    for ( unsigned i=2; i<= d ; i++ )
    {
      string add = h[i];
      if ( add.size() )
      {
        ret+=h[i];
        if ( i != d ) ret+=":";
      };
    }

    // cout << "[dbg] createSub d=" << d << " ret=" << ret << endl;
    return ret;
  }
}

bool XmlReader::openFile()
{
  if ( theFileName=="" ) return true;
  try {
    theReader = new xmlpp::TextReader ( theFileName );
    hasRead=false;
    return true;
  } catch  ( ... ) {
    return false;
  }
}

XmlReader::XmlReader ( const XmlReader & o ) :
  AbstractReader(o),
  theReader ( 0 ), hasRead(false),
  theFileName ( o.theFileName )
{
  if (!openFile() )
  {
    cout << "[XmlReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

XmlReader::~XmlReader()
{
  if ( theReader ) delete theReader;
}

XmlReader::XmlReader( string filename ) :
  AbstractReader(),
  theReader(0), hasRead(false), theFileName(filename)
{
  if (!openFile() )
  {
    cout << "[XmlReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

string XmlReader::filename() const
{
  return theFileName;
}

XmlReader * XmlReader::clone ( const string & file ) const
{
  return new XmlReader ( file );
}

int XmlReader::getNumberOfEntries() const
{
  return -1;
}

void XmlReader::reset()
{
  delete ( theReader );
  theReader=0;
  openFile();
}

void XmlReader::addSubTuple ( Tuple & t, const string & prefix ) const
{
  if ( prefix == "#text" ) return; // what on earth?
  // cout << "[AddSubTuple] adding " << prefix << endl;

  string pre = prefix;
  if ( pre.size() ) pre+=":";

  if (theReader->has_attributes())
  {
    theReader->move_to_first_attribute();
    do
    {
      MultiType m  ( theReader->get_value() );
      m.trimType();
      t[pre + theReader->get_name()]=m;
    } while (theReader->move_to_next_attribute());
    theReader->move_to_element();
  }
}

Tuple XmlReader::next ()
{
  if ( !hasRead )
  {
    if (!theReader->read() ) throw TerminateException( );
  }

  // skip tuples with no attributes
  while ( theReader->get_attribute_count() == 0 )
  {
    if (!theReader->read() ) throw TerminateException( );
  }

  string name = theReader->get_name();

  Tuple t ( name );
  addSubTuple ( t, "" );

  int dpth=1;
  map < int, string > hierarchies;
  int lastdpth=0;
  string prefix="";

  while ( dpth>0 )
  {
    // read subtuples
    hasRead = theReader->read();
    if ( !hasRead ) break; // no more data? break off 
    int dpth = theReader->get_depth();
    // cout << "[dbg] " << theReader->get_name() << " d=" << dpth << endl;
    if ( dpth > 1 )
    {
      string sub = theReader->get_name(); 
      if ( sub=="#text" ) continue;
      hierarchies[dpth]=sub;
      if ( prefix.size() && lastdpth>=dpth )
      {
        // fill the last one!
        // cout << "-             filling " << prefix << endl;
        t.fill(prefix,0);
      };

      prefix = createSub ( hierarchies, dpth );
      addSubTuple ( t, prefix );
    } else {
      // next tuple has started already
      hasRead=true;
      break;
    }
    lastdpth=dpth;
  }

  if ( prefix.size() && lastdpth>=dpth )
  {
    // fill the last one!
    // cout << "-             filling " << prefix << endl;
    t.fill(prefix,0);
  };
  // cout << "[XmlReader] now filling and returning" << endl;
  t.fill();

  Tuple ret = postProcess ( t );
  CommentsCollector coll;
  coll.manipulate ( ret );
  return ret;
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<XmlReader> t ( "xml" );
}

