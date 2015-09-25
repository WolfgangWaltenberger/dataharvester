#ifndef RootHelpers_H
#define RootHelpers_H

#include <dataharvester/MultiType.h>
#include <dataharvester/Tuple.h>
#include <string>
#include <vector>

class TLeaf;
class TBranch;
class TTree;

namespace dataharvester {
class RootHelpers {
public:
  /**
   *  \class RootHelpers
   *  A few helper methods.
   */

  /// Get/set current tree.
  static void setCurTree ( TTree * );
  static TTree * curTree();

  /// get all values from branch, without recursion!
  static std::vector < MultiType > getValues ( TBranch *, int offset=0 );

  /// get a short version of the branch name
  /// (cut out <prefix>, if necessary)
  static std::string getShortBranchname ( TBranch * branch, const std::string & prefix );
  static std::string getShortLeafname ( TLeaf * branch, const std::string & prefix );

  /// get all nested data, store it in vector < map >
  /// @paramname offset, start from <offset> entry
  static std::vector < std::map < std::string, MultiType > > getNested ( TBranch * branch,
      const std::string & prefix, int offset=0 );
  /// print this thingie
  static void printNested ( const std::vector < std::map < std::string, MultiType > > & m );

  static void ignoreWarnings();

private:
  /// return type name, as a MultiType (tricky, huh?)
  static MultiType typeName ( const std::string & type );

  /// get a single value from leaf
  static MultiType getValue ( const TLeaf * leaf, const std::string & type, int entry=0, int offset=0 );

  /// get a single value from leaf, implicit typing
  static MultiType getValue ( const TLeaf * leaf, int entry=0 );

  /// get all values from leaf
  static std::vector < MultiType > getValues ( const TLeaf * leaf, int offset=0 );

  /// add row <row> of <data> to <t> (with a few checks)
  static void addData ( Tuple & t, const std::vector < MultiType > & data, 
                        unsigned row, const std::string & name );

};
}

#endif
