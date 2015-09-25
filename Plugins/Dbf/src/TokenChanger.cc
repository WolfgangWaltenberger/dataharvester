#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/TokenChanger.h>
#include <dataharvester/SimpleConfigurable.h>

using namespace std;
using namespace dataharvester;

namespace {
  int lower_case ( int c )
  {
    return tolower ( c );
  }

  int transform_special_char ( int c )
  {
    while ( c < 0 )
    {
      c+=256;
    };
    switch (c)
    {
      case 129: // ~A
        return (int) '�';
      case 130: // ~B
        return (int) '�';
      case 132: // ~D
        return (int) '�';
      case 142: // ~N
        return (int) '�';
      case 148: // ~T
        return (int) '�';
      case 153: // ~Y
        return (int) '�';
      case 154: // ~Z
        return (int) '�';
      case 225: // �
        return (int) '�';
      case 239:
        return (int) '\'';
      default:
         return c;
    };
  }

  void to_lowercase ( MultiType & in )
  {
    if ( in.isType() != MultiType::kString ) return;
    string lowercase=in.asString();
    transform ( lowercase.begin(), lowercase.end(), lowercase.begin(), lower_case );

    in=lowercase;
  }

  bool all_lowercase()
  {
    bool ret = SimpleConfigurable<bool> 
      ( true, "TokenChanger:AllLowercase" ).value();
    return ret;
  }

  bool is_dbf_umlaute()
  {
    bool ret = SimpleConfigurable<bool> 
      ( true, "TokenChanger:DbfUmlaute" ).value();
    return ret;
  }

  void dbf_umlaute ( MultiType & in )
  {
    if ( in.isType() != MultiType::kString ) return;
    string lowercase=in.asString();
    transform ( lowercase.begin(), lowercase.end(), lowercase.begin(), transform_special_char );

    in=lowercase;
  }
}

TokenChanger::TokenChanger()
{
}

MultiType TokenChanger::transform ( const MultiType & in, TokenType t )
{
  MultiType ret=in;
  if ( t != kValue && all_lowercase () )
    to_lowercase ( ret );
  if ( is_dbf_umlaute() )
    dbf_umlaute ( ret );
  return ret;
}

