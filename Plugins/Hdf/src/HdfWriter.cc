#include <dataharvester/HarvestingConfiguration.h>
#include "HdfWriter.h"
#include <dataharvester/Helpers.h>
#include <dataharvester/Mode.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/Cout.h>
#include <fstream>
#include <sstream>
#include <ctime>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <dataharvester/SimpleConfigurable.h>
#include <dataharvester/TupleRow.h>
#include "HdfReader.h"
#include "HdfHelpers.h"

using namespace std;
using namespace dataharvester;
using namespace H5;
  // /usr/share/doc/libhdf5-doc/examples/c++/h5group.cpp.gz
  // /usr/share/doc/libhdf5-doc/examples/c++/compound.cpp.gz

namespace
{
  int deflate()
  {
    return SimpleConfigurable<int>(9,"HdfWriter:Deflate").value();
  }

  // determines actual string length, given a suggestion.
  // takes into account maxStringLength.
  int string_length ( int suggestion )
  {
    int ret = suggestion;
    int max = Helpers::maxStringLength();
    if ( ret == -1 || ret > max ) ret=max;
    return ret;
  }

  int verbose()
  {
    return 0;
  }

  vector < char > specialChars()
  {
    static vector < char > ret;
    if ( !ret.size() )
    {
      ret.push_back ( ',' );
      ret.push_back ( ';' );
//      ret.push_back ( ':' );
    };
    return ret;
  }

  string escapeSpecialChars ( string in, bool value_is_vector = false )
  {
    vector < char > scs = specialChars();
    bool had_change = false;
    do {
      had_change = false;
      for ( vector< char >::const_iterator schr=scs.begin();
            schr!=scs.end() ; ++schr ) // schr = special char
      {
        if ( value_is_vector && *schr == ',' ) continue;
        for ( string::iterator pos=in.begin(); pos!=in.end(); ++pos )
        {
          if ( *pos == *schr )
          {
            if ( pos == in.begin() || *(pos-1) != '\\' )
            {
              in.insert ( pos, 1, '\\' );
              had_change=true;
              break;
            };
          };
        };
        if ( had_change ) break;
      };
    } while ( had_change );
    return in;
  }

  pair < string, string > split_names ( string tuplename )
  {
    string::size_type pos = tuplename.rfind("/");
    if ( pos == string::npos )
    {
      return pair < string, string > ( "", tuplename );
    };
    try {
      string group=tuplename.substr(0,pos );
      string name=tuplename.substr(pos+1,tuplename.size() );
      return pair < string, string > ( group, name );
    } catch ( ... ) {};
    cout << "[HdfWriter] silly programming error in " << __LINE__ << endl;
    exit(-1);
  }

  void writeVersionString ( const Group & dt )
  {
    // now write down the title
    {
      StrType cls ( PredType::C_S1, 8 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "TITLE", cls, cld );
      clattr.write ( cls, "dataharvester" );
    };

    // write down the class
    {
      StrType cls ( PredType::C_S1, 6 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "CLASS", cls, cld );
      // clattr.write ( cls, "TABLE" );
      clattr.write ( cls, "GROUP" );
    };

    // now write down the version
    {
      StrType cls ( PredType::C_S1, 4 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "VERSION", cls, cld );
      clattr.write ( cls, "1.0" );
    };

    // and now the pytables version
    {
      StrType cls ( PredType::C_S1, 4 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "PYTABLES_FORMAT_VERSION", cls, cld );
      clattr.write ( cls, "1.4" );
    };
  }

  void writeStaticInfo ( const DataSet & dt, string description )
  {
    // now write down the title
    {
      StrType cls ( PredType::C_S1, 8 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "TITLE", cls, cld );
      clattr.write ( cls, "dataharvester" );
    };
    // write down the class
    {
      StrType cls ( PredType::C_S1, 6 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "CLASS", cls, cld );
      clattr.write ( cls, "TABLE" );
      // clattr.write ( cls, "GROUP" );
    };
    // now write down the version
    {
      StrType cls ( PredType::C_S1, 4 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "VERSION", cls, cld );
      clattr.write ( cls, "2.2" );
    };

    // and now for a description
    if ( description.size() )
    {
      StrType cls ( PredType::C_S1, description.size()+1 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          "DESCRIPTION", cls, cld );
      clattr.write ( cls, description );
    };
  }

  int writeFieldInfo ( const DataSet & dt, const map < string, MultiType > & amp, int ct=0 )
  {
    for ( map < string, MultiType >::const_iterator
          i=amp.begin(); i!=amp.end() ; ++i )
    {
      ostringstream os;
      os << "FIELD_" << ct << "_NAME";
      string name=i->first;
      StrType cls ( PredType::C_S1, name.size()+1 );
      DataSpace cld ( H5S_SCALAR  );
      Attribute clattr=dt.createAttribute (
          os.str(), cls, cld );
      clattr.write ( cls, name );

      /*
      if ( description.size() )
      {
        // If we have a description (a comment),
        // we write it down
        ostringstream os2;
        os2 << "FIELD_" << ct << "_DESCRIPTION";
        StrType cls_d ( PredType::C_S1, description.size()+1 );
        DataSpace cld_d ( H5S_SCALAR  );
        Attribute clattr_d=dt.createAttribute (
            os2.str(), cls_d, cld_d );
        clattr_d.write ( cls_d, description );
      };*/
      ct++;
    };
    return ct;
  }

  int writeFieldInfo ( const DataSet & dt, const TupleRow & row, int ct=0 )
  {
    map < string, MultiType > simple = row.getSimpleData();
    ct=writeFieldInfo ( dt, simple, ct );
    map < string, Tuple * > nested = row.getNestedData();

    map < string, MultiType > nests;
    for ( map < string, Tuple * >::const_iterator i=nested.begin();
          i!=nested.end() ; ++i )
    {
      vector < TupleRow > rows = i->second->getTupleRows();
      if ( rows.size() )
      {
        nests[i->first]=i->second->getName();
        // ct=writeFieldInfo ( dt, rows[0],ct );
      };
    };
    writeFieldInfo ( dt, nests, ct );
    return 0;
  }

  int computeBytes ( const map < string, MultiType > & amp )
  {
    int n_bytes=0;
    for ( map < string, MultiType >::const_iterator i=amp.begin();
          i!=amp.end() ; ++i )
    {
      // so we can compare if the layout changes later on
      if ( i->second.isType() == MultiType::kString ||
           i->second.isType() == MultiType::kDouble32Vec ||
           i->second.isType() == MultiType::kDouble64Vec ||
           i->second.isType() == MultiType::kInt32Vec ||
           i->second.isType() == MultiType::kInt64Vec )
      {
        int lngth = Helpers::extractLength ( i->first ).second;
        n_bytes+=string_length ( lngth );
      } else {
        n_bytes+=HdfHelpers::sizeOf ( i->second, i->first );
      };
    };
    return n_bytes;
  }

  int computeBytes ( const TupleRow & row )
  {
    int ret=computeBytes ( row.getSimpleData() );

    map < string, Tuple * > nested=row.getNestedData();
    for ( map < string, Tuple * >::const_iterator i=nested.begin();
          i!=nested.end() ; ++i )
    {
      vector < TupleRow > nested = i->second->getTupleRows();
      for ( vector< TupleRow >::const_iterator j=nested.begin();
            j!=nested.end() ; ++j )
      {
        // ret+=4;
        ret+=computeBytes ( *j );
      };
    };
    return ret;
  }
}

void HdfWriter::flatsave ( const TupleRow & r,
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
      flatsave ( *j, tuplename+"_"+name, prefix );
    }
  }
}

void HdfWriter::save ( const TupleRow & r,
                       const string & tuplename, const string & prefix )
{
  if ( Helpers::dontSave() ) return;
  createGroup ( tuplename );

  if ( Helpers::flatData() )
  {
    flatsave ( r, tuplename, prefix );
    return;
  };

  if ( tuplename != "" && !theHasDataSet[tuplename] )
  {
    createDataSet ( r, tuplename, "" );
  };

  bool chk = theChecker.check ( r, tuplename );
  if (!chk)
  {
    cout << "[HdfWriter] skipping data row!" << endl;
    return;
  };

  fillRawData ( r, tuplename );
}

void HdfWriter::save ( const Tuple & d, const string & prefix )
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

  if ( !d.getCurrentRow().isEmpty() )
  {
    save ( d.getCurrentRow(), name, prefix );
  }
}

HdfWriter::~HdfWriter()
{
  for ( map < string, void * >::iterator i=theRawData.begin();
        i!=theRawData.end() ; ++i )
  {
    free ( i->second );
  };
  for ( map < string, DataSpace * >::iterator i=theDataSpace.begin();
        i!=theDataSpace.end() ; ++i )
  {
    delete i->second;
  };

  for ( map < string, CompType * >::iterator i=theCompType.begin();
        i!=theCompType.end() ; ++i )
  {
    delete i->second;
  };
  for ( map < string, DataSet * >::iterator i=theDataSet.begin();
        i!=theDataSet.end() ; ++i )
  {
    hsize_t dims[] = { 1 };
    DataSpace cld ( 1, dims );
    Attribute rattr=(*(i->second)).createAttribute ( "NROWS", PredType::NATIVE_INT, cld );
    long s=theNRows[i->first];
    rattr.write ( PredType::NATIVE_UINT64, (void *) (&s) );
    delete i->second;
  };
  for ( map < string, Group * >::iterator i=theGroup.begin();
        i!=theGroup.end() ; ++i )
  {
    delete i->second;
  };
  if ( theFile )
  {
    delete theFile;
    theFile=0;
  };
}

void HdfWriter::checkExistance()
{
  // check if file exists. if not, we change file mode
  // from update to recreate
  FILE * f = fopen ( theFileName.c_str(), "r" );
  if ( !f )
  {
    Mode::setFileMode ( Mode::Recreate );
  };
}

void HdfWriter::copyOldData ( const string & old )
{
  HdfReader seeder ( old );
  try {
    while ( true )
    {
      Tuple tuple = seeder.next();
      save ( tuple );
    }
  } catch ( dataharvester::TerminateException & ) {};
}

HdfWriter::HdfWriter ( const string & filename ) :
  theFileName ( filename ), theFile(0)
{
  if ( Helpers::dontSave() ) return;
  if ( theFileName == "" ) return;
  unsigned flags = H5F_ACC_TRUNC;

  if ( Mode::fileMode() == Mode::Update )
  {
    checkExistance ();
  }

  if ( Mode::fileMode() == Mode::Recreate )
  {
    // move old file
    Helpers::createBackup ( filename, false );
  }

  theFile = new H5File ( theFileName.c_str(), flags );
  Group main = theFile->openGroup("/");
  writeVersionString ( main );
  if ( Mode::fileMode() == Mode::Update )
  {
    string tmpfile=Helpers::backupName( filename );
    copyOldData ( tmpfile );
    // finally, remove the tmp file
    unlink ( tmpfile.c_str() );
  };
}

void HdfWriter::save ( const map < string, MultiType > & amp,
    const string & tuplename )
{
  if ( Helpers::dontSave() ) return;
  createGroup ( tuplename ); // create group, if we need to

  if ( tuplename != "" && !theHasDataSet[tuplename] )
  {
    // create dataset if we need to
    createDataSet ( amp, tuplename, "" );
  };

  bool chk = theChecker.check ( amp, tuplename );
  if (!chk) return;

  // fill data
  fillRawData ( amp, tuplename );
}

void HdfWriter::createGroup ( const string & tuplename )
{
  pair < string, string > group_and_tuple =
    split_names ( tuplename );
  string group=group_and_tuple.first;
  string tuple=group_and_tuple.second;

  /*
  cout << "[debug] here filemode=" << Mode::fileMode() << endl;
  cout << "                    g=" << group << endl;
  */
  if ( Mode::fileMode() == Mode::Update 
       && group !="" && !theHasGroup[group] )
  {
    try
    {
      string gname="/"+group;
      Group tmp = theFile->openGroup ( gname.c_str() );
      cout << "[debug] whow. group opened." << endl;
      theGroup[group] = new Group ( tmp );
      return;
    } catch ( GroupIException & ) {
      // ok, cannot open. doesnt exist. so we create it.
    }
  }

  if ( group != "" && !theHasGroup[group])
  {
    string gname="/"+group;
    theGroup[group]=new Group(
        theFile->createGroup ( gname.c_str() ) );
    theHasGroup[group]=true;
  }
}

void HdfWriter::createDataSet ( const map < string, MultiType > & amp,
                      const string & tuplename, const string & description )
{

  // FIXME. If file mode is update, then open existing data set.
  pair < string, string > group_and_tuple =
    split_names ( tuplename );
  string group=group_and_tuple.first;
  string tuple=group_and_tuple.second;
  // string gname = "/" + tuplename;
  string gname = tuplename;
  // int nfields=amp.size();
  hsize_t dim[] = { 1 };
  hsize_t maxdims[] = { H5S_UNLIMITED };

  theChecker.define ( amp, tuplename );
  int n_bytes=computeBytes ( amp );
  // cout << "[HdfWriter] n_bytes=" << n_bytes << endl;

  /*
   * Modify dataset creation properties, i.e. enable chunking.
   */
  DSetCreatPropList cparms;
  hsize_t      chunk_dims[] ={ n_bytes*10 };
  cparms.setChunk( 1, chunk_dims );
  cparms.setDeflate ( deflate() );

  theDataSpace[tuplename] = new DataSpace ( 1, dim, maxdims );
  // theCompType[tuplename] = new CompType ( (size_t) n_bytes );
  // cout << "- new comptype " << n_bytes << endl;
  // createCompType ( amp, tuplename );

  int offset=0;
  theCompType[tuplename]=createCompType ( amp, offset );

  theRawData[tuplename]=malloc ( n_bytes );
  if ( Helpers::verbose() > 9 )
  {
    cout << "[HdfWriter] allocating total of " << n_bytes
         << " bytes for " << tuplename << " at " << theRawData[tuplename]
         << endl;
  };

  theDataSet[tuplename]=new DataSet(
      theFile->createDataSet ( gname,
        *(theCompType[tuplename]), *(theDataSpace[tuplename]), cparms ) );

  writeStaticInfo ( *(theDataSet[tuplename]), description );
  writeFieldInfo ( *(theDataSet[tuplename]), amp );
  theDataSet[tuplename]->extend ( dim );
  theHasDataSet[tuplename]=true;
  theNRows[tuplename]=0;
}

void HdfWriter::createDataSet (
    const TupleRow & row, const string & tuplename, const string & description )
{
  theChecker.define ( row, tuplename );

  map < string, MultiType > amp = row.getSimpleData();

  // string gname="/"+tuplename;
  string gname=tuplename;

  hsize_t dim[] = { 1 };
  hsize_t maxdims[] = { H5S_UNLIMITED };


  int n_bytes=computeBytes ( row );

  /*
   * Modify dataset creation properties, i.e. enable chunking.
   */
  DSetCreatPropList cparms;
  hsize_t      chunk_dims[] ={ n_bytes*10 };
  cparms.setChunk( 1, chunk_dims );
  cparms.setDeflate ( deflate() );

  /*
   * Set fill value for the dataset
   */
  // int fill_val = 0;
  // cparms.setFillValue( PredType::NATIVE_INT, &fill_val);

  theDataSpace[tuplename] = new DataSpace ( 1, dim, maxdims );
  /*
  int offset=-1;
  theCompType[tuplename]= createCompType ( amp, offset );
  */

  theCompType[tuplename] = new CompType ( (size_t) n_bytes );
  // int offset=createCompType ( row, tuplename );

  theRawData[tuplename]=malloc ( n_bytes );
  if ( Helpers::verbose() > 9 )
  {
    cout << "[HdfWriter] allocating total of " << n_bytes
         << " bytes for " << tuplename << " at " << theRawData[tuplename]
         << endl;
  };
  theDataSet[tuplename]=new DataSet(
      theFile->createDataSet ( gname,
        *(theCompType[tuplename]), *(theDataSpace[tuplename]), cparms ) );

  writeStaticInfo ( *(theDataSet[tuplename]), description );
  writeFieldInfo ( *(theDataSet[tuplename]), row );
  theDataSet[tuplename]->extend ( dim );
  theHasDataSet[tuplename]=true;
  theNRows[tuplename]=0;
}

int HdfWriter::insertCompType ( const string & name, const MultiType & value,
                                       const CompType & tp, int offset )
{
  // returns amount of bytes
  if ( value.isType() == MultiType::kString ||
       value.isType() == MultiType::kDouble32Vec ||
       value.isType() == MultiType::kDouble64Vec ||
       value.isType() == MultiType::kInt32Vec ||
       value.isType() == MultiType::kInt64Vec )
  {
    if ( ! ( value.isType() == MultiType::kString ) )
    {
      cout << "[HdfWriter] WARNING: Treating Vecs as strings!" << endl;
    };
    pair < string, int > col_n_lngth = Helpers::extractLength( name );
    int lngth=col_n_lngth.second;
    int real_lngth = string_length ( lngth );
    StrType st ( PredType::C_S1, real_lngth );
    // StrType st ( real_lngth );
    /*
    IntType * t = new IntType();;
    VarLenType st (  t ); */
    // StrType st2 ( PredType::C_S1, 1 );
    // VarLenType st ( &st2 );
    // StrType st ( PredType::C_S1, H5T_VARIABLE );
    tp.insertMember( col_n_lngth.first, (size_t) offset, st );
    if ( Helpers::verbose() > 9 )
    {
      cout << "[HdfWriter] allocating " << real_lngth << " bytes for "
           << col_n_lngth.first << endl;
    };
    return real_lngth;
    // return 1;
  } else {
    HdfHelpers::insertRightType
      ( &tp, name, (size_t) offset, value );
    if ( Helpers::verbose() > 9 )
    {
      cout << "[HdfWriter] allocating " << HdfHelpers::sizeOf ( value )
           << " bytes for " << name << endl;
    };
    return HdfHelpers::sizeOf ( value, name );
  };
}

int HdfWriter::createCompType ( const map < string, MultiType > & data,
    const string & tuplename )
{
  int offset=0;
  for ( map < string, MultiType >::const_iterator i=data.begin();
        i!=data.end() ; ++i )
  {
    int nb=insertCompType ( i->first, i->second, *(theCompType[tuplename]), offset );
    offset+=nb;
  };
  return offset;
}

int HdfWriter::createCompType ( const TupleRow & row, const string & tuplename )
{
  int offset=0;
  map < string, MultiType > data=row.getSimpleData();
  for ( map < string, MultiType >::const_iterator i=data.begin();
        i!=data.end() ; ++i )
  {
    int nb=insertCompType ( i->first, i->second, *(theCompType[tuplename]), offset );
    offset+=nb;
  };
  addNestedData ( row.getNestedData(), tuplename, offset );

  return offset;
}


H5::CompType * HdfWriter::createCompType ( const map < string, MultiType > & data, int & length )
{
  int n_bytes=computeBytes ( data );
  length=n_bytes;
  CompType * ret = new CompType ( (size_t) n_bytes );
  int offset=0;
  for ( map < string, MultiType >::const_iterator i=data.begin();
        i!=data.end() ; ++i )
  {
    int nb=insertCompType ( i->first, i->second, *(ret), offset );
    offset+=nb;
  };
  return ret;
}

H5::CompType * HdfWriter::createCompType ( const TupleRow & row, int & length )
{
  int n_bytes=computeBytes ( row );
  length=n_bytes;
  CompType * ret = new CompType ( (size_t) n_bytes );
  int offset=0;
  map < string, MultiType > data = row.getSimpleData();
  for ( map < string, MultiType >::const_iterator i=data.begin();
        i!=data.end() ; ++i )
  {
    int nb=insertCompType ( i->first, i->second, *(ret), offset );
    offset+=nb;
  };
  return ret;
}

int HdfWriter::addNestedData (
    const map < string, Tuple * > & data, const string & tuplename, int offset )
{
  for ( map < string, Tuple * >::const_iterator i=data.begin(); i!=data.end() ; ++i )
  {
    vector < TupleRow > nested = i->second->getTupleRows();
    int nrows=nested.size();
    if ( nrows==0 ) continue;

    hsize_t h[]={ nrows }; // FIXME

    map < string, MultiType > sd = nested[0].getSimpleData();
    int l;
    H5::CompType * tp = createCompType ( sd, l );
    // FIXME need to keep *tp
    // cout << "[ArrayType] size=" << tp->getSize() << endl;
    ArrayType arr ( *tp, 1 /* rank 1 */, h );
    // VarLenType arr ( tp );
    // ok, array must be array of the right CompTypes.
    // hid_t id = theCompType[tuplename]->getId();
    theCompType[tuplename]->insertMember ( i->first, offset, arr );

    // offset needs to be updated

    offset+=tp->getSize() * nrows; // FIXME

    //cout << "[HdfWriter] addNestedData: adding " << tp->getSize() * nrows << " bytes."
    //     << endl;

    offset= addNestedData ( nested[0].getNestedData(), tuplename, offset );
    delete tp; // FIXME? Is that a Prob?
  }
  return offset;
}

int HdfWriter::copyRawData ( const map < string, MultiType > & amp, 
    const string & tuplename, int c )
{
  // cout << "[HdfWriter] copying raw data at " << c << endl;
  // theRawData is filled
  for ( map < string, MultiType >::const_iterator i=amp.begin();
        i!=amp.end() ; ++i )
  {
    // MultiType::Type tp=i->second.isType();
    int lngth=Helpers::extractLength(i->second).second;
    switch ( i->second.isType() )
    {
      case MultiType::kString:
      case MultiType::kDouble32Vec:
      case MultiType::kDouble64Vec:
      case MultiType::kInt32Vec:
      case MultiType::kInt64Vec:
      {
        string si=i->second.asString();
        char * mtrx = (char *) theRawData[tuplename];
        string firstn=si.substr(0,string_length ( lngth )-1 );
        if ( Helpers::verbose() > 11 )
        {
          cout << "[HdfWriter] writing " << firstn << " to " << c << endl;
        };
        for ( unsigned int pc=0; pc< firstn.size()  ; ++pc )
        {
          mtrx[pc+c]=firstn[pc];
        };
        mtrx[c+firstn.size()]='\0';
        int lngth = Helpers::extractLength ( i->first ).second;
        c+=string_length (lngth ) *sizeof(char);
        break;
      };
      case MultiType::kDouble32:
      {
        float si=i->second.asDouble();
        char * mtrx = (char *) theRawData[tuplename];
        mtrx+=c;
        memcpy ( mtrx, &si, sizeof(float) );
        // mtrx[c]=si;
        c+=sizeof(float);
        break;
      };
      case MultiType::kDouble64:
      {
        double si=i->second.asDouble();
        char * mtrx = (char *) theRawData[tuplename];
        mtrx+=c;
        // cout << "[HdfWriter] getting double at " << c << endl;
        memcpy ( mtrx, &si, sizeof(double) );
        // mtrx[c]=si;
        c+=sizeof(double);
        break;
      };
      case MultiType::kBool:
      {
        int si=i->second.asBool();
        char * mtrx=(char *) theRawData[tuplename];
        mtrx+=c;
        memcpy ( mtrx, &si, sizeof(int) );
        c+=sizeof(int);
        break;
      };

      case MultiType::kInt32:
      case MultiType::kInt64:
      default:
      {
        int si=i->second.asInt();
        char * mtrx=(char *) theRawData[tuplename];
        mtrx+=c;
        memcpy ( mtrx, &si, sizeof(int) );
        c+=sizeof(int);
        break;
      };
    };
  };
  return c;
}

int HdfWriter::fillRawData ( const TupleRow & r,
    const string & tuplename, int c )
{
  // cout << "[HdfWriter] filling raw data c=" << c << endl;
  c=copyRawData ( r.getSimpleData(), tuplename, c );
  // cout << "[HdfWriter] after simple: " << c << endl;
  map < string, Tuple * > more=r.getNestedData();
  for ( map < string, Tuple * >::const_iterator i=more.begin();
        i!=more.end() ; ++i )
  {
    // cout << "[HdfWriter] now copying " << i->second->getName() << endl;
    vector < TupleRow > rows=i->second->getTupleRows();
    for ( vector< TupleRow >::const_iterator j=rows.begin();
          j!=rows.end() ; ++j )
    {
      map < string, MultiType > data = j->getSimpleData();
      c=copyRawData ( data, tuplename, c );
    };
  };
  // cout << "[HdfWriter] now writing raw data" << endl;
  writeRawData ( tuplename );
  return c;
}

void HdfWriter::writeRawData ( const string & tuplename )
{

  // extend the dataset
  theNRows[tuplename]++;
  hsize_t dim[] = { theNRows[tuplename], 1 };
  theDataSet[tuplename]->extend ( dim );

  /*
   * Select a hyperslab.
   */

  DataSpace fspace2 = theDataSet[tuplename]->getSpace ();

  // H5_VERS_MAJOR 1
  // H5_VERS_MINOR 6
  // H5_VERS_RELEASE 3-4

  #if H5_VERS_MINOR < 8
  #if H5_VERS_RELEASE < 4
  hssize_t offset[1]; // hdf5 < 1.6.4
  #else
  hsize_t offset[1]; // hdf5 > 1.6.3
  #endif
  #else
  hsize_t offset[1]; // hdf5 >= 1.8.0
  #endif

  offset[0] = theNRows[tuplename]-1;
  // offset[1] = 0;
  hsize_t adim[] = { 1 };
  fspace2.selectHyperslab( H5S_SELECT_SET, adim , offset );
  DataSpace mspace2 ( 1, adim );

  theDataSet[tuplename]->write ( theRawData[tuplename],
      *(theCompType[tuplename]), mspace2, fspace2 );
}

void HdfWriter::fillRawData ( const map < string, MultiType > & amp,
                                     const string & tuplename )
{
  int c=0;
  copyRawData ( amp, tuplename, c );
  writeRawData ( tuplename );
}

HdfWriter * HdfWriter::clone( const string & filename ) const
{
  return new HdfWriter ( filename );
}

#include <dataharvester/WriterPrototypeBuilder.h>
namespace {
  WriterPrototypeBuilder<HdfWriter> t2( "hdf, h5" );
}

