#include <dataharvester/HarvestingConfiguration.h>
#include "RootWriter.h"
#include <dataharvester/MultiType.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <dataharvester/Helpers.h>
#include <dataharvester/SimpleConfigurable.h>

using namespace std;
using namespace dataharvester;

namespace
{
  int flushAfter()
  {
    static int ret = SimpleConfigurable < int >
              ( 100000, "RootWriter:FlushAfter" ).value();
    return ret;
  }

  bool merge()
  {
    static bool ret = SimpleConfigurable < bool > 
             ( true, "RootWriter:Merge" ).value();
    return ret;
  }

  bool append()
  {
    static string ret = SimpleConfigurable < string > 
             ( "Recreate", "Mode:FileMode" ).value();
    std::transform( ret.begin(), ret.end(), ret.begin(), ::tolower);
    static bool append = ( ret == "append" );
    return append;
  }
}

void RootWriter::write()
{
  if ( theIsClosed )
  {
    cout << "[RootWriter] Warning: trying to write to closed file - "
         << "ignoring request." << endl;
    return;
  };
  theCache.write();
  theItemsInCache=0;
}

void RootWriter::save ( const Tuple & d, const string & prefix )
{
  // const_cast <Tuple &> (d).fill ( "", false );
  vector < TupleRow > rows=d.getTupleRows();
  string name=d.getName();
  for( vector< TupleRow >::iterator i = rows.begin(); 
       i != rows.end(); i++)
  { // print one tuple row
    // cout << "  `-- dumping tuplerow" << endl;
    save ( *i, name, prefix );
  }
  const TupleRow & cur = d.getCurrentRow();
  if ( !cur.isEmpty() )
  {
    save ( cur, name, prefix );
  }
}

void RootWriter::save ( const TupleRow & r,
                        const string & tuplename, const string & prefix )
{
  map < string, MultiType > simple=r.getSimpleData();
  save ( simple, prefix+tuplename );
  map < string, Tuple * > nested=r.getNestedData();
  for ( map < string, Tuple * >::const_iterator i=nested.begin(); 
        i!=nested.end() ; ++i )
  {
    vector < TupleRow > rows=i->second->getTupleRows();
    string name=i->second->getName();
    for ( vector< TupleRow >::const_iterator j=rows.begin(); 
          j!=rows.end() ; ++j )
    {
      save ( *j, tuplename+"_"+name, prefix );
    }
  }
}

void RootWriter::save ( const map < string, MultiType > & mp, const string & name )
{
  if ( Helpers::dontSave() ) return;
  if (!Helpers::checkTupleName ( name ) )
  {
    cout << "[DataHarvesting] error: tuplename " << name << " is not valid!"
         << endl;
    cout << "[DataHarvesting] skipping this tuple!" << endl;
    return;
  };

  if ( Helpers::verbose() > 10 )
  {
    cout << "[RootWriter] adding dataharvester request: " << theFileName
         << ", " << name << endl;
  };
  if ( theIsClosed )
  {
    cout << "[RootWriter] Warning: trying to write to closed file - "
         << "ignoring request." << endl;
  };
  theCache.add ( name, mp, "" );
  theCache.setNeedsSave ();
  theItemsInCache++;

  if ( theItemsInCache >= flushAfter() )
  {
    theFlushed=true;
    write();
  };
}

RootWriter::~RootWriter()
{
  if ( !theIsClosed )
  {
    theCache.close();
    theIsClosed=true;
    theItemsInCache=0;
    if ( merge() && ( theFlushed || append() ) )
    {
      // if merging is on and we are in append mode or its flushing time,
      // we merge!
      ostringstream ostr;
      ostr << "dataharvester_merge -i " << theFileName << " -o " << theFileName 
           << endl;
      // cout << "[RootWriter] " << ostr.str() << endl;
      system ( ostr.str().c_str() );
    }
  };
}

RootWriter::RootWriter ( const string & filename ) : 
  theItemsInCache (0), theIsClosed ( false ), theFileName ( filename ),
  theFlushed ( false )
{
  theCache.setFileName ( filename );
}

RootWriter * RootWriter::clone ( const string & filename ) const
{
  return new RootWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < RootWriter, WriterManager::FileFormat > t ( "root" );
}

