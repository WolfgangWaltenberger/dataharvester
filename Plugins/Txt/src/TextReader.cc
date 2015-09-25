#include <dataharvester/HarvestingConfiguration.h>
#include "TextReader.h"
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
  int verbose()
  {
    return 0;
  }

  void stripQuotations ( string & s )
  {
    if ( s[0] == '\"' || s[0] == '\'' )
    {
      s.replace ( 0, 1, "" );
    };
    if ( s[s.size()-1] == '\"' || s[s.size()-1] == '\'' )
    {
      s.replace ( s.size()-1, 1, "" );
    };
  }

  /// remove everything after a "#".
  void remove_comments ( string & line )
  {
    string::size_type pos = line.find("#");
    if ( (signed int) pos != (signed int) -1 )
    {
      int nq = StringTools::number_quotes ( line, pos );
      if ( fmod ( nq, (float) 2.0 ) < 0.001 )
      {
        // not quoted
        line.replace ( pos, line.size(), "" );
      };
    };

    while ( line.size() )
    {
      string::size_type pos = line.find(" ");
      if ( pos != 0 ) break;
      line.replace ( 0,1,"" );
    };
  }

  bool checkForEnd ( string s, string & tuplename )
  {
    string::size_type pos = s.find("ends");
    if ( pos == s.size()-4 )
    {
      tuplename = s.substr ( 0,pos-1 );
      // tuplename+= "ends";
      return true;
    };
    return false;
  }
}

bool TextReader::openFile()
{
  if ( theStream != 0 ) return true;
  if ( theFileName == "" ) return true;
  if ( theFileName == "stdin" || theFileName == "cin" || theFileName == "--" )
  {
    theStream = &(std::cin);
    theStreamNeedsDeletion = false;
  } else {
    ifstream * tmp = new ifstream ( theFileName.c_str() );
    theStream = tmp;
    theStreamNeedsDeletion = true;
    if ( !tmp->is_open() )
    {
      return false;
    };
  };
  return true;
}

TextReader::TextReader ( const TextReader & o ) : 
  StreamableReader ( o ),
  theFileName ( o.theFileName ), theStream ( 0 ),
  theStreamNeedsDeletion ( o.theStreamNeedsDeletion ),
  theLineNr ( 0 )
{
  if (!openFile() )
  {
    cout << "[TextReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

TextReader::~TextReader()
{
  if ( theStream && theStreamNeedsDeletion ) delete theStream;
  theStream=0;
}

TextReader::TextReader( string filename ) :
  StreamableReader(),
  theFileName(filename), theStream(0),
  theStreamNeedsDeletion(true), theLineNr(0)
{
  if (!openFile() )
  {
    cout << "[TextReader] Fatal: file " << theFileName
         << " cannot be opened." << endl;
    throw;
  };
}

TextReader::TextReader ( istream * s ) :
  StreamableReader(), theLineNr(0)
{
  setStream ( s, false );
}

void TextReader::setStream ( istream * s, bool del )
{
  theFileName="";
  theStream=s;
  theStreamNeedsDeletion=del;
}

bool TextReader::parseLine ( string line, string & nname,
       map < string, MultiType > & mp )
{
  StringTools::strip ( line );
  if ( line.size() == 0 ) return true;
  string::size_type pos = StringTools::find_unescaped ( line,":" );
  if ( pos == string::npos )
  {
    cout << "[TextReader] Line without ``:'' -- skipping!" << endl;
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
      cout << "[TextReader] " << s_ntpl << " in  line " << theLineNr <<
              " lacks an equal (``='') sign " << endl;
      return false;
    };
    string name=s_ntpl.substr(0,equals);
    string value="undef";
    try {
      value=s_ntpl.substr( equals+1 );
    } catch (...) {
      cout << "[TextReader] error: could not get value for ``" << name << "''"
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
    cout << "[TextReader] /parse " << endl;
  };
  return true;
}

string TextReader::filename() const
{
  return theFileName;
}

TextReader * TextReader::clone ( const string & file ) const
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
  return new TextReader ( file );
}

int TextReader::getNumberOfEntries() const
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

bool TextReader::getLine ( string & ret ) const
{
  ret="";
  char tmp;
  while ( theStream->get ( tmp ) )
  {
    if ( tmp == '\n' )
    {
      /*
      if ( ret.size() > 1900 )
      {
        cout << "[TextReader] got line with n size " << ret.size() << endl;
      }*/
      return true;
    };
    ret+=tmp;
  }

  /*
  if ( ret.size() > 1900 )
  {
    cout << "[TextReader] got line with size " << ret.size() << endl;
  }*/
  return false;
}

bool TextReader::nextNtuple( string & ntuplename,
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
    cout << "[TextReader] warning: no eof yet" << endl;
  };
  return false;
}

bool TextReader::nextNtuple( string & ntuplename,
                             map < string, MultiType > & mp, string sline )
{
  theLineNr++;
  if ( sline == "__END__" )
  {
    cout << "[TxtFileReader] END marker found." << endl;
    return false;
  };
  remove_comments ( sline );
  if ( sline.size() )
  {
    string::size_type pos = StringTools::find_unescaped ( sline, ":");
    if ( pos == string::npos )
    {
      bool end = checkForEnd ( sline,ntuplename );
      if ( end ) {
        // "flu.h> end marker found.
        // Produce flush signal
        mp.clear();
        mp["ends"]=true;
        return true;
      } else {
        cout << "[TextReader] line without ``:'' found -- ignoring line."
             << endl;
      };
    } else {
      return parseLine ( sline, ntuplename, mp );
    };
  };
  return true;
}

Tuple TextReader::getCurrentTuple()
{
  Tuple t = postProcess ( theCurrentTuple );
  CommentsCollector coll;
  coll.manipulate ( t );
  return t;
  // return theCurrentTuple;
}

void TextReader::reset()
{
  // theStream->seekg(0,ios::beg);
  theLineNr=0;
  if ( theStreamNeedsDeletion ) delete theStream;
  theStream=0;
  openFile();
}

Tuple TextReader::next ()
{
  string line;
  while ( true )
  {
    if ( getLine ( line ) )
    {
      theLineNr++;
      remove_comments ( line );
      line=StringTools::strip ( line );
      bool added=addLine ( line );
      if (added)
      {
        Tuple ret=theCurrentTuple;
        theCurrentTuple=Tuple();
        Tuple t = postProcess ( ret );
        CommentsCollector coll;
        coll.manipulate ( t );
        return t;
      };
    } else {
      bool eof = theStream->eof();
      if ( eof != true )
      {
        cout << "[TextReader] warning: no eof yet" << endl;
      };
      throw  TerminateException();
    };
  };

  // shouldnt land here
  throw  TerminateException();
}

bool TextReader::addLine ( const string & lin )
{
  string line(lin);
  if ( line.size() == 0 ) { return false; };
  // cout << "[dbg] adding \"" << line << "\"" << endl;
  // add a line. return true if Tuple is finished.

  string::size_type pos = StringTools::find_unescaped ( line, ":");
  if ( pos == string::npos )
  {
    cout << "[TextReader] line without : - skipping ... " << endl;
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

bool TextReader::addToTuple ( string line )
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
    cout << "[TextReader] Warning: line ends neither with a ';' nor with a '.' nor with a ':'"
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
    // cout << "[TextReader] filling -" << ostr.str() << "-" << endl;
    theCurrentTuple.fill ( ostr.str() );
    // exit(0);
    if ( scs.size()==1 ) return true;
    return false;
  }
  return false;
}

bool TextReader::addData ( string line )
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
      cout << "[TextReader] no equal sign found in data line "
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
    // cout << "[TextReader:dbg]  `- adding " << prefix + name << "=" << value << endl;
    theCurrentTuple [ prefix + name ] = value;
  };
  return true;
}

void TextReader::clear()
{
  theCurrentTuple.clear();
}

#include <dataharvester/ReaderPrototypeBuilder.h>

namespace {
  ReaderPrototypeBuilder<TextReader> t ( "txt" );
}

