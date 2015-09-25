#include <dataharvester/HarvestingConfiguration.h>
#include "DhfWriter.h"
#include "DhfHelpers.h"
#include "MultiTypeBinaryConverter.h"
#include <dataharvester/Helpers.h>
// #include <dataharvester/CommentsExpresser.h>
#include <dataharvester/Mode.h>
#include <fstream>

using namespace dataharvester;
using namespace std;

void DhfWriter::save ( const Tuple & d, const string & prefix)
{
  // const_cast < Tuple & > (d).fill("",false);
  if ( !theHasTupleDescription [ prefix + d.getName() ] )
  {
    describeTuple ( d, prefix );
  }

  saveTuple ( d, prefix );
}

void DhfWriter::describeTuple ( const Tuple & d, const string & prefix )
{
  if ( d.getNumberOfRows() == 0 )
  {
    cout << "[DhfWriter] warning: ignoring empty tuple " << d.getName();
    return;
  }
  // this method should stream out a description of the tuple with all columns and all.
  (*theStream) << DhfHelpers::prototype << d.getName() << DhfHelpers::separator << d.getDescription();

  const TupleRow & row = d.getRow(0);
  const map < string, MultiType > & simple = row.getSimpleData();
  for ( map < string, MultiType >::const_iterator i=simple.begin(); i!=simple.end() ; ++i )
  {
    (*theStream) << DhfHelpers::comma
                 << DhfHelpers::toChar ( i->second.isType() )
                 << i->first 
                 << DhfHelpers::separator << row.getDescription ( i->first );
  }

  const map < string, Tuple * > & nested = row.getNestedData();
  for ( map< string, Tuple * >::const_iterator i=nested.begin(); 
        i!=nested.end() ; ++i )
  {
    (*theStream) << DhfHelpers::comma << DhfHelpers::is_prototype << i->first 
                 << DhfHelpers::separator << row.getDescription ( i->first );
  }
      
  (*theStream) << DhfHelpers::newline;

  for ( map< string, Tuple * >::const_iterator i=nested.begin(); 
        i!=nested.end() ; ++i )
  {
    if (!theHasTupleDescription [ prefix+i->first+i->second->getName() ] )
    {
      describeTuple ( *(i->second), prefix+i->first );
    }
  }


  theHasTupleDescription[prefix+d.getName()]=true;
}

void DhfWriter::saveTuple ( const Tuple & d, const string & prefix )
{
  int n = d.getNumberOfRows();
  // string sprefix=prefix;
  // if ( sprefix.size() ) sprefix+=":";
  for ( int i=0; i< n ; i++ )
  {
    saveTupleRow ( d.getRow(i), /* sprefix+ */ d.getName() );
  }

  if ( !d.getCurrentRow().isEmpty() )
  {
    saveTupleRow ( d.getCurrentRow(), d.getName() );
  }
}

void DhfWriter::saveTupleRow ( const TupleRow & row, const string & prefix )
{
  (*theStream) << DhfHelpers::data << prefix;
  const map < string, MultiType  > & data = row.getSimpleData();
  for ( map < string, MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
  {
    (*theStream) << DhfHelpers::comma << MultiTypeBinaryConverter::toBinary ( i->second );
  }

  const map < string, Tuple * > & nested = row.getNestedData();
  for ( map < string, Tuple * >::const_iterator j=nested.begin(); j!=nested.end() ; ++j )
  {
    (*theStream) << DhfHelpers::comma;
    saveTuple ( *(j->second), prefix );
  }

  (*theStream) << endl;
}

void DhfWriter::setStream ( ostream & o, bool needs_deletion )
{
  if ( theStreamNeedsDeletion )
  {
    delete theStream;
  };
  theStreamNeedsDeletion=needs_deletion;
  theStream=&o;
}

DhfWriter::~DhfWriter()
{
  if ( theStream && theStreamNeedsDeletion )
  {
    delete theStream;
  };
}

DhfWriter::DhfWriter ( const string & filename ) :
  theFileName ( filename ), theStream ( 0 ), theStreamNeedsDeletion ( true )
{
  if ( Helpers::dontSave() ) return;
 
  if ( theFileName == "" ) return;
  if ( theFileName == "--.dhf" || theFileName == "stdout.dhf" ||
       theFileName == "cout.dhf" )
  {
    setStream ( cout, false );
  } else {
    ios_base::openmode mode = ofstream::app;
    if ( Mode::fileMode() == Mode::Recreate )
    {
      mode=ofstream::trunc;
    };
    setStream ( *(new ofstream ( theFileName.c_str(), mode | ios::binary )), true );
  };

  (*theStream) << "DHF0.1" << DhfHelpers::newline;

}

DhfWriter::DhfWriter ( ostream & o ) :
  theFileName ( "" ), theStream ( &o ), theStreamNeedsDeletion ( false )
{}

DhfWriter * DhfWriter::clone( const string & filename ) const
{
  return new DhfWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder
    < DhfWriter, WriterManager::FileFormat > t ( "dhf" );
}

