#ifndef TupleRowConsistencyChecker_H
#define TupleRowConsistencyChecker_H

#include <string>
#include <map>
#include <dataharvester/MultiType.h>
#include <dataharvester/TupleRow.h>

namespace dataharvester {
class TupleRowConsistencyChecker {
public:
  /**
   *  \class TupleRowConsistencyChecker
   *  Helper class that checks if data are consistent.
   */
  
  TupleRowConsistencyChecker ();
  void define ( const std::map < std::string, MultiType > &, std::string tuplename="" );
  bool check ( const std::map < std::string, MultiType > &, std::string tuplename="" );

  void define ( const TupleRow & row, std::string tuplename="" );
  bool check ( const TupleRow & row, std::string tuplename="" );

private:
  std::map < std::string, bool > isDefined;
  std::map < std::string, std::map < std::string, MultiType::Type > > theTemplate;
};
}

#endif
