#include <dataharvester/HarvestingConfiguration.h>
#include "HdfReader.h"
#include <dataharvester/HarvestingException.h>
#include <dataharvester/TerminateException.h>
#include "H5File.h"
#include "H5Cpp.h"
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <string.h>

// /usr/share/doc/libhdf5-doc/examples/c++/compound.cpp.gz
// /usr/share/doc/libhdf5-doc/examples/c++/readdata.cpp.gz

using namespace H5;
using namespace std;
using namespace dataharvester;

namespace {
  void debug ( TupleRow & row )
  {
    map < string, MultiType > data = row.getSimpleData();
    for ( map < string, MultiType >::iterator i=data.begin(); 
          i!=data.end() ; ++i )
    {
      i->second.trimType();
      cout << "  -- " << i->first << "=" << i->second.asString()
           << "  " << i->second.isA()
           << endl;
    }
  }

  void debug ( Tuple & tuple )
  {
    cout << "[HdfReader] dumping tuple " << endl;
      tuple.dump();
      vector < TupleRow  > data=tuple.getTupleRows();
      for ( vector< TupleRow >::iterator i=data.begin(); 
            i!=data.end() ; ++i )
      {
        debug ( *i );
      }
  }

  int verbose()
  {
    return 0;
  }

  string pretty_print ( float s )
  {
    ostringstream ostr;
    int d=int ( log10(s) / 3 );
    switch (d)
    {
      case 0:
        ostr << s;
        break;
      case 1:
        ostr << s / 1000. << "K";
        break;
      case 2:
        ostr << s/1000000. << "M";
        break;
      case 3:
        ostr << s/1000000000. << "G";
        break;
      case 4:
      default:
        ostr << s/1000000000000. << "T";
        break;
    };
    return ostr.str();
  }

  vector < string > retrieveColumnNames ( const DataSet * s )
  {
    // FIXME currently using attributes information
    // should look for a more direct way ...
    vector < string > ret;
    int m=s->getNumAttrs();
    cout << "[HdfReader] the data set has " << m << " attributes." << endl;
    for ( int i=0; i< m ; i++ )
    {
      Attribute tmp=s->openAttribute(i);
      DataType dtype=tmp.getDataType();
      string name=tmp.getName();
      if ( name.find("FIELD_") != string::npos && name.find("_NAME") != string::npos )
      {
        string value="???";
        DataSpace attrspace=tmp.getSpace();
        tmp.read(dtype,value);
        // cout << "Value: :" << value << ":" << endl;
        ret.push_back ( value );
      };
    };
    return ret;
  }

  void print ( const map < string, MultiType > & mp )
  {
    for ( map < string, MultiType >::const_iterator i=mp.begin();
          i!=mp.end() ; ++i )
    {
      cout << i->first << ": " << i->second.asString() << "  ";
    };
    cout << endl;
  }
}

void HdfReader::reset ()
{
  theCurrentRow=0;
  theCurrentGroup="";
  theCurrentGroupIdx=0;
  theNRows=0;
  theMaxGroupNr=-1;
  // cout << "[HdfReader] resetting" << endl;
  if ( theRaw ) free ( theRaw );
  if ( theFile ) delete theFile;
  theRaw=0;
  theFile=0;
  if (!openFile() )
  {
    cout << "[HdfReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
  // cout << "[HdfReader] /resetting" << endl;
  // cout << "[HdfReader] reset not implemented" << endl;
}

map < string, MultiType > HdfReader::retrieveRow ( int r )
{
  // cout << "[HdfReader] retrieving row " << r << endl;
  map < string, MultiType > ret;
  CompType ctype = theDataSet.getCompType();
  int csize=ctype.getSize();

  int nentries=ctype.getNmembers();

  // cout << "[HdfReader] reading next row" << endl;
  for ( int i=0; i< nentries ; i++ )
  {
    MultiType mt=getMultiType ( ctype, i, r*csize );
    // cout << "[HdfReader::retrieveRow] mt=" << mt << " isA=" << mt.isA() << endl;
    string name=ctype.getMemberName ( i );
    ret[name]=mt;
    // cout << "[HdfReader] " << name << "=" << mt << " (" << mt.isA() << ")" << endl;
  };
  // free(theRaw);
  // theAllocCounter-=ssize;
  return ret;
}

MultiType HdfReader::getMultiType ( const CompType & ctype, int idx, int row_offset )
{
  // returns the value  of row row_offset at index idx,
  // given a buffer raw.
  DataType mem_type=ctype.getMemberDataType ( idx );
  int offset=ctype.getMemberOffset ( idx ) + row_offset;
  string name=ctype.getMemberName ( idx );
  MultiType mt;

  int swtch=mem_type.getClass();
  // bool det=mem_type.detectClass( H5Tget_class ( H5T_NATIVE_INT ) );

  long pt=(long) (theRaw);
  int dsize=mem_type.getSize();

  if ( mem_type.detectClass(  H5Tget_class ( H5T_NATIVE_INT ) ) )
  {
    long int x=0;
    memcpy ( (void *) &x, (void *) (pt+offset), dsize );
    mt=x;
    // cout << "[HdfReader] mt= int" << endl;
  } else if ( mem_type.detectClass ( H5Tget_class ( H5T_NATIVE_FLOAT ) ) )
  {
    size_t s = H5Tget_size ( mem_type.getId() );
    if ( s == 4 )
    {
      float x=0.;
      memcpy ( (void *) &x, (void *) (pt+offset), dsize );
      mt=x;
      // cout << "[HdfReader] a float! s=" << s << " x=" << x << endl;
    } else if ( s == 8 ) {
      double x=0.;
      memcpy ( (void *) &x, (void *) (pt+offset), dsize );
      mt=x;
      // cout << "[HdfReader] a float! s=" << s << " x=" << x << endl;
    } else {
      cout << "[HdfReader] ERROR: a float-type variable of size " << s << endl;
      cout << "[HdfReader] I am not prepared for this!" << endl;
      mt=-.1;

    }
  } else if ( mem_type.detectClass ( H5Tget_class ( H5T_C_S1 ) ) )
  {
    // cout << "[HdfReader] String detected!" << endl;
    // string x="???";

    // cout << "[HdfReader] size=" <<dsize << endl;
    void * x= malloc ( dsize );
    memcpy ( x, (void *) (pt+offset), dsize );
    mt=(char *) x;
    free(x);
  } else {
    cout << "[HdfReader] error! dont know this type "
         << swtch << endl;
    exit(-1);
  };
  mt.trimType();
  // cout << "[HdfReader] mt=" << mt << "  " << mt.isA() << endl;
  return mt;
}

bool HdfReader::readRow( map < string, MultiType > & mp )
{
  // map < string, MultiType > news = readRow ( s, theCurrentRow );
  mp = retrieveRow ( theCurrentRow );
  // cout << "[HdfReader] has read row" << endl;
  // print ( mp );
  theCurrentRow++;
  return true;
}

bool HdfReader::openFile()
{
  if ( theFileName == "" ) return true;
  if ( theFile )
  {
    cout << "[HdfReader] error: trying to open file \""
         << theFileName << "\" again?!?" << endl;
    return false;
  };
  try {
    theFile = new H5File ( theFileName.c_str(), H5F_ACC_RDONLY );
    // set the max group number
    theMaxGroupNr=theFile->getNumObjs();
    theCurrentGroupIdx=0;
    getNewGroup();
    return true;
  } catch ( FileIException & error ) {
    error.printError();
  } catch ( ... ) {
    cout << "[HdfReader] error: unknown exception caught" << endl;
  };
  return false;
}

HdfReader::~HdfReader()
{
  if ( theRaw ) free ( theRaw );
  if ( theFile ) delete theFile;
}
HdfReader::HdfReader( const HdfReader & o ) :
  AbstractReader ( o ),
  theFileName ( o.theFileName ) , theCurrentGroup(""),
  theCurrentGroupIdx(0), theNRows(0), theCurrentRow(0),
  theMaxGroupNr(-1), theFile(0), theRaw(0)
{
  if ( theFileName == "" ) return;
  if (!openFile() )
  {
    cout << "[HdfReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
  // cout << "[HdfReader] " << theFileName << " opened." << endl;
}

HdfReader::HdfReader( const string & filename ) :
  AbstractReader(),
  theFileName ( filename ) , theCurrentGroup(""),
  theCurrentGroupIdx(0), theNRows(0), theCurrentRow(0),
  theMaxGroupNr(-1), theFile(0), theRaw(0)
{
  if ( theFileName == "" ) return;
  if (!openFile() )
  {
    cout << "[HdfReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
  // cout << "[HdfReader] " << theFileName << " opened." << endl;
}

HdfReader * HdfReader::clone ( const string & file ) const
{
  // FIXME file open should be done once not twice
  ifstream i ( file.c_str() );
  if ( !i.is_open() )
  {
    return 0;
  };
  i.close();
  return new HdfReader ( file );
}

string HdfReader::filename() const
{
  return theFileName;
}

int HdfReader::getNumberOfEntries() const
{
  int n=0;
  string tmp;
  for ( int i=0; i< theMaxGroupNr ; i++ )
  {
    theFile->getObjnameByIdx( (hsize_t) i, tmp, 255 );
    H5::DataSet ds = theFile->openDataSet( tmp.c_str() );
    DataSpace dataspace = ds.getSpace();
    int tmp=(int) (dataspace.getSelectNpoints () ); // gives you rows
    n+=tmp;
  }
  return n;
}

bool HdfReader::getNewGroup()
{
  if ( theCurrentGroupIdx >= theMaxGroupNr )
    return false;
  theFile->getObjnameByIdx( (hsize_t) theCurrentGroupIdx, theCurrentGroup, 255 );
  // cout << "[HdfReader] getting new group \"" << theCurrentGroup << "\"" << endl;
  theDataSet = theFile->openDataSet( theCurrentGroup.c_str() );
  DataSpace dataspace = theDataSet.getSpace();
  theNRows=(int) (dataspace.getSelectNpoints () ); // gives you rows
  theCurrentGroupIdx++;
  theCurrentRow=0;
  if (theRaw)
  {
    free(theRaw);
    theRaw=0;
  };

  // get the whole thing at once!
  // FIXME would be nice if the read could be split up
  // Use Regions?!?
  CompType ctype = theDataSet.getCompType();
  int csize=ctype.getSize();
  DataSpace space=theDataSet.getSpace();
  int np=space.getSimpleExtentNpoints ();
  int ssize=np*csize;
  if ( ssize > 50000000 )
  {
    cout << "[HdfReader] Warning: allocated " << pretty_print( ssize ) << "B at once."
         << endl;
    cout << "[HdfReader] write better Hdf backends!" << endl;
  }
  theRaw = malloc ( ssize );

  // FIXME would be nice if the read could be split up
  // Use Regions?!?
  theDataSet.read ( theRaw, ctype );

  return true;
}

bool HdfReader::nextNtuple( string & ntuplename,
                                 map < string, MultiType > & mp )
{
  if ( theCurrentRow >= theNRows )
  {
    bool ret=getNewGroup();
    if (!ret)
    {
      ntuplename="";
      mp.clear();
      return ret;
    };
  };
  ntuplename=theCurrentGroup;
  readRow( mp );
  return true;
}

Tuple HdfReader::next ( )
{
  string tuplename="???";
  map < string, MultiType > data;
  bool ret = nextNtuple ( tuplename, data );
  if ( ret )
  {
    Tuple t ( tuplename );
    for ( map < string, MultiType >::const_iterator i=data.begin(); i!=data.end() ; ++i )
    {
      t[i->first]=i->second;
    };
    t.fill();
    return postProcess ( t );
    return t;
  } else {
    throw TerminateException ();
  };
}


#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<HdfReader> t ( "hdf, h5" );
}

