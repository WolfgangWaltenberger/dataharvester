#include <dataharvester/HarvestingConfiguration.h>
#include "DhfReader.h"
#include "DhfHelpers.h"
#include "MultiTypeBinaryConverter.h"
#include <dataharvester/TerminateException.h>
#include <dataharvester/CommentsCollector.h>
#include <dataharvester/StringTools.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cstdlib>

using namespace dataharvester;
using namespace std;

namespace {
  pair < string, string > nameAndComment ( const string & s )
  {
    pair < string, string > ret;
    list < string > namecomment = StringTools::split ( s, DhfHelpers::separator );
    if ( namecomment.size() > 0 )
    {
      ret.first=namecomment.front();
      namecomment.pop_front();
    } else {
      ret.first=s;
    }
    if ( namecomment.size() )
    {
      ret.second=namecomment.front();
    }
    return ret;
  }

  int verbose()
  {
    return 0;
  }
}

bool DhfReader::openFile()
{
  if ( theStream != 0 ) return true;
  if ( theFileName == "" ) return true;
  if ( theFileName == "stdin.dhf" || theFileName == "cin.dhf" || theFileName == "--.dhf" )
  {
    theStream = &(std::cin);
    theStreamNeedsDeletion = false;
  } else {
    ifstream * tmp = new ifstream ( theFileName.c_str(), ios::binary );
    theStream = tmp;
    theStreamNeedsDeletion = true;
    if ( !tmp->is_open() )
    {
      return false;
    };
  };

  readSignature();
  return true;
}

bool DhfReader::readSignature() const
{
  string first;
  bool ret = getLine ( first );
  // cout << "[DhfReader] Signature: " << first << endl;
  if ( first.find("DHF")!= 0 )
  { 
    cout << "[DhfReader] Error: file is not a DHF file." << endl;
    exit(0);
  }
}

DhfReader::DhfReader ( const DhfReader & o ) : 
  AbstractReader ( o ),
  theFileName ( o.theFileName ), theStream ( 0 ),
  theStreamNeedsDeletion ( o.theStreamNeedsDeletion ),
  theLineNr ( 0 )
{
  if (!openFile() )
  {
    cout << "[DhfReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

DhfReader::~DhfReader()
{
  if ( theStream && theStreamNeedsDeletion ) delete theStream;
  theStream=0;
}

DhfReader::DhfReader( string filename ) :
  AbstractReader(),
  theFileName(filename), theStream(0),
  theStreamNeedsDeletion(true), theLineNr(0)
{
  if (!openFile() )
  {
    cout << "[DhfReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

DhfReader::DhfReader ( istream * s ) :
  AbstractReader(), theLineNr(0)
{
  setStream ( s, false );
}

void DhfReader::setStream ( istream * s, bool del )
{
  theFileName="";
  theStream=s;
  theStreamNeedsDeletion=del;
}

bool DhfReader::parseLine ( string line, string & nname,
       map < string, MultiType > & mp )
{
  StringTools::strip ( line );
  if ( line.size() == 0 ) return true;
  string::size_type pos = StringTools::find_unescaped ( line,":" );
  if ( pos == string::npos )
  {
    cout << "[DhfReader] Line without ``:'' -- skipping!" << endl;
    return true;
  };
  string ntuplename = line.substr(0,pos);
  map < string, MultiType > ntuple;

  line.replace(0,pos+1,"" );
  StringTools::lstrip ( line );
  while ( line.size() )
  {
    string::size_type comma = line.find(";");
    if ( comma == string::npos ) comma = line.size();
    string s_ntpl = line.substr(0,comma);
    if ( s_ntpl.size() == 0 )
    {
      return true; // false;
    };
    line.replace(0,comma+1,"");
    StringTools::lstrip ( line );
    // eraseLeadingSpaces ( line );
    string::size_type equals = s_ntpl.find("=");
    if ( equals == string::npos )
    {
      cout << "[DhfReader] " << s_ntpl << " in  line " << theLineNr <<
              " lacks an equal (``='') sign " << endl;
      return false;
    };
    string name=s_ntpl.substr(0,equals);
    string value="undef";
    try {
      value=s_ntpl.substr( equals+1 );
    } catch (...) {
      cout << "[DhfReader] error: could not get value for ``" << name << "''"
           << endl;
    };
    try {
      ntuple[name]=value;
      // fixing the type and trying an up-conversion...
      ntuple[name].trimType();
      // cout << "[TDS debug] ntuple=" << ntuple[name] << " is type " << ntuple[name].typeName ( ntuple[name].isType() ) << endl;
    } catch (...) { cout << "What?!?" << endl; };
  };
  nname = ntuplename;
  mp = ntuple;
  if ( verbose() > 0 )
  {
    cout << "[DhfReader] /parse " << endl;
  };
  return true;
}

string DhfReader::filename() const
{
  return theFileName;
}

DhfReader * DhfReader::clone ( const string & file ) const
{
  if ( file != "stdin" && file != "cin" && file != "--" )
  {
    // FIXME file open should be done once not twice
    ifstream i ( file.c_str() );
    if ( !i.is_open() )
    {
      return 0;
    };
    i.close();
  };
  return new DhfReader ( file );
}

int DhfReader::getNumberOfEntries() const
{
  ifstream i ( theFileName.c_str() );
  if ( !i.is_open() )
  {
    return 0;
  };
  int ret=0;
  string line;
  while ( getLine ( line ) )
  {
    ret++;
  };
  i.close();
  return ret;
}

bool DhfReader::getLine ( string & ret ) const
{
  ret="";
  char tmp;
  while ( theStream->get ( tmp ) )
  {
    if ( tmp == DhfHelpers::newline )
    {
      return true;
    };
    ret+=tmp;
  }

  return false;
}

bool DhfReader::nextNtuple( string & ntuplename,
                             map < string, MultiType > & mp )
{
  string line;
  if ( getLine ( line ) )
  {
    return nextNtuple ( ntuplename, mp, line );
  };
  bool eof = theStream->eof();
  if ( eof != true )
  {
    cout << "[DhfReader] warning: no eof yet" << endl;
  };
  return false;
}

bool DhfReader::nextNtuple( string & ntuplename,
                             map < string, MultiType > & mp, string sline )
{
  theLineNr++;
  return true;
}

Tuple DhfReader::getCurrentTuple()
{
  Tuple t = postProcess ( theCurrentTuple );
  CommentsCollector coll;
  coll.manipulate ( t );
  return t;
  // return theCurrentTuple;
}

void DhfReader::reset()
{
  // theStream->seekg(0,ios::beg);
  theLineNr=0;
  if ( theStreamNeedsDeletion ) delete theStream;
  theStream=0;
  openFile();
}

Tuple DhfReader::next ()
{
  theCurrentTuple.clear();
  string line;
  while ( true )
  {
    if ( getLine ( line ) )
    {
      if ( line.find( DhfHelpers::prototype ) == 0 )
      {
        defineTuple ( line );
      }
      if ( line.find( DhfHelpers::data ) == 0 )
      {
        readTupleRow ( line );
        return theCurrentTuple;
      }
    } else {
      bool eof = theStream->eof();
      if ( eof != true )
      {
        cout << "[DhfReader] warning: no eof yet" << endl;
      };
      throw  TerminateException();
    };
  };

  // shouldnt land here
  throw TerminateException();
}

// read one tuple row.
void DhfReader::readTupleRow ( string line, const string & prefix )
{
  line=line.substr(1,string::npos);
  list < string > tokens = StringTools::split ( line, DhfHelpers::comma );
  if ( tokens.size() == 0 ) return;
  if ( theEmptyTuples.count ( tokens.front() ) == 0 )
  {
    cout << "[DhfReader] ERROR: got no dhf prototype for " << tokens.front()
         << "! data loss is possible!" << endl;
    return;
  }

  Tuple t = theEmptyTuples[tokens.front()];
  theCurrentTuple.setName ( t.getName() );
  theCurrentTuple.setDescription ( t.getDescription() );
  tokens.pop_front();
  // cout << "[DhfReader] found." << endl; // << t.contains() << endl;
  
  // now fill "theCurrentTuple" with values from "tokens",
  // in the shape of "t".

  const map < string, MultiType > & simple = t.getRow(0).getSimpleData();

  for ( map< string, MultiType >::const_iterator i=simple.begin(); 
        i!=simple.end() ; ++i )
  {
    if ( tokens.size() == 0 )
    {
      return;
    }

    if ( tokens.front().size() == 0 ) continue;

    if ( tokens.front()[0] == DhfHelpers::data )
    {
      string name = tokens.front();
      name=name.replace(0,1,"");
      // we need to create a subtuple for this!
      theCurrentTuple[name+":status"]="create";
      // readSubTuple ( simple, i, name );
    } else {
      // copy data
      theCurrentTuple[prefix+i->first]=MultiTypeBinaryConverter::toMultiType ( tokens.front().c_str(),
          i->second.isType() );
      // copy type information
      theCurrentTuple[prefix+i->first].defineAs ( i->second.isType() );
      if ( t.getDescription ( i->first ).size() )
        theCurrentTuple.describe ( t.getDescription ( i->first), prefix+i->first );
    }
    tokens.pop_front();
  }

  theCurrentTuple.fill(prefix,false);
}

void DhfReader::defineTuple ( string line )
{
  // remove first.
  line=line.substr(1,string::npos);
  // cout << "[DhfReader] define a tuple: " << line << endl;
  list < string > tokens = StringTools::split ( line, DhfHelpers::comma );
  Tuple t;
  if ( tokens.size() )
  {
    pair < string, string > nac = nameAndComment ( tokens.front () );
    t.setName ( nac.first );
    if ( nac.second.size() ) t.setDescription ( nac.second );
    tokens.pop_front();
  }

  for ( list < string >::const_iterator i=tokens.begin(); i!=tokens.end() ; ++i )
  {
    if ( i->size() == 0 ) continue;
    string token = i->substr(1,string::npos);
    MultiType::Type type = DhfHelpers::toType ( (*i)[0] );
    pair < string, string > nac = nameAndComment ( token );

    t[nac.first].defineAs ( type );
    if ( nac.second.size() )
    {
      // cout << "[DhfReader] describe " << nac.second << "-" << nac.first << endl;
      t.describe(nac.second,nac.first);
    }
  }

  t.fill();

  theEmptyTuples[t.getName()]=t;

  // cout << "[DhfReader] " << t.contains() << endl;
}

bool DhfReader::addLine ( const string & lin )
{
  string line(lin);
  if ( line.size() == 0 ) { return false; };
  // cout << "[dbg] adding \"" << line << "\"" << endl;
  // add a line. return true if Tuple is finished.

  string::size_type pos = StringTools::find_unescaped ( line, ":");
  if ( pos == string::npos )
  {
    cout << "[DhfReader] line without : - skipping ... " << endl;
    return false;
  };
  string tuplename=line.substr(0,pos);

  // cout << "[dbg] tuplename=>" << tuplename << "<" << endl;
  if ( !(theCurrentTuple.getName() == tuplename ) )
  {
    // FIXME should do an automatic ::fill if necessary!
    // theCurrentTuple.fill();
    theCurrentTuple=Tuple ( tuplename );
  };
  line=line.substr(pos+1,string::npos);
  line=StringTools::lstrip ( line );
  return addToTuple ( line );
}

bool DhfReader::addToTuple ( string line )
{
  if ( line.size() == 0 ) return false;

  list < string > scs=StringTools::split ( line, ":" );

  if ( scs.size()==1 && *(scs.begin())=="fill" )
  {
    // top level fill!
    // cout << "[dbg] filling " << theCurrentTuple.getName() << endl;
    theCurrentTuple.fill();
    return true;
  }
  
  if ( scs.size() && *(scs.rbegin())=="fill" )
  {
    string::size_type npos=StringTools::rfind_unescaped ( line, ":");
    string what=line.substr(0,npos);
    // cout << "[dbg] filling \"" << what << "\" from " << line << "!!!" << endl;
    theCurrentTuple.fill ( what );
    return false;
  };

  if ( line[line.size()-1] != '.' && line[line.size()-1] !=';' && line[line.size()-1] !=':' )
  {
    cout << "[DhfReader] Warning: line ends neither with a ';' nor with a '.' nor with a ':'"
         << endl
         << "                 assuming a ';'"
         << endl;
    cout << "             Length: " << line.size() << endl;
    cout << "             line=\"" << line << "\"" << endl;
    exit(0);
  }
  // cout << "[dbg] adding >" << line << "< to tuple \"" << line << "\"" << endl;
  bool ends_with_point=false;
  if ( line[line.size()-1]=='.' )
  {
    line=line.substr(0,line.size()-1);
    // cout << "line :" << line << ":" << endl;
    ends_with_point=true;
  }

  addData ( line );
  if ( ends_with_point && scs.size()>0 )
  {
    ostringstream ostr;
    list<string>::const_iterator end=scs.end();
    end--; // end--;
    for ( list <string >::const_iterator i=scs.begin(); i!=end ; ++i )
    {
      if (i!=scs.begin())
      {
        ostr << ":";
      }
      ostr << *i;
    }
    // cout << "[DhfReader] filling -" << ostr.str() << "-" << endl;
    theCurrentTuple.fill ( ostr.str() );
    // exit(0);
    if ( scs.size()==1 ) return true;
    return false;
  }
  return false;
}

bool DhfReader::addData ( string line )
{
  string prefix="";
  string::size_type pos = StringTools::rfind_unescaped ( line, ":" ) ;

  if ( pos != string::npos )
  {
    prefix=line.substr(0,pos)+":";
    line=line.substr(pos+1,string::npos );
    StringTools::lstrip ( line );
  };
  
  list < string > tokens = StringTools::split ( line, ";" );
  for ( list < string >::const_iterator i=tokens.begin(); 
        i!=tokens.end() ; ++i )
  {
    string::size_type pos = i->find("=");
    if ( pos == string::npos )
    {
      cout << "[DhfReader] no equal sign found in data line "
           << theLineNr << "! Skipping" << endl;
      cout << "  `- line was " << line << endl;
      cout << "  `- prefix was " << prefix << endl;
      continue;
    };
    string name=i->substr(0,pos);
    string svalue=i->substr(pos+1,string::npos );
    StringTools::strip(svalue);
    bool was_quoted = StringTools::removeQuotes ( svalue );
    MultiType value=svalue;
    if ( was_quoted ) value.defineAs( MultiType::kString );
    value.trimType();
    StringTools::strip(name);
    // cout << "[DhfReader:dbg]  `- adding " << prefix + name << "=" << value << endl;
    theCurrentTuple [ prefix + name ] = value;
  };
  return true;
}

void DhfReader::clear()
{
  theCurrentTuple.clear();
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<DhfReader> t ( "dhf" );
}

