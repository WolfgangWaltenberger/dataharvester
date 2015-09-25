#ifndef StringTools_H
#define StringTools_H

#include <list>
#include <string>
//#include <cctype>

namespace dataharvester {
namespace StringTools {

  //* eat up leading spaces (~= pythons std::string.lstrip())
  std::string & lstrip( std::string & source );
  //* eat up trailing spaces (~= pythons std::string.rstrip())
  std::string & rstrip( std::string & source );
  //* eat up leading and trailing spaces
  std::string & strip( std::string & source );

  //* return position of unescaped substd::string in src std::string
  //*   searching from front
  std::string::size_type 
    find_unescaped ( const std::string & src, const std::string & sub );
  //* return position of unescaped substd::string in src std::string
  //*   searching from back
  std::string::size_type 
    rfind_unescaped ( const std::string & src, const std::string & sub );

  //* return list of stripped substd::strings, 
  //*   split up on unescaped ( not '\X') separator std::string 'X'
  //* \paramname num quit splitting if num>0 and number of found substrings
  //* greater or equal num.
  std::list< std::string > 
    split ( std::string source, std::string sep = " ", unsigned int num=0);
  std::list< std::string > 
    split ( std::string source, char sep, unsigned int num=0);

  std::pair < std::string, std::string > splitPath ( 
      const std::string & path );

  bool removeQuotes ( std::string & ret );
  int number_quotes ( const std::string & src, std::string::size_type pos );
}
}

#endif

