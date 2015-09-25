#ifndef dataharvester_UniversalManipulator_H
#define dataharvester_UniversalManipulator_H

#include <dataharvester/Manipulator.h>
#include <string>
#include <vector>
#include <map>
#include <set>

namespace dataharvester {
class UniversalManipulator : public Manipulator {
public:
  /** \class UniversalManipulator
   *  A UniversalManipulator can drop
   *  subtuples and columns,
   *  and rename subtuples and columns.
   */

  /// Drop subtuples with this name.
  void dropTuple ( const std::string & name );

  /// Drop columns with this name.
  void dropColumn ( const std::string & name );

  /// Rename column.
  void renameColumn ( const std::string & From, const std::string & to );

  /// Rename tuple.
  void renameTuple ( const std::string & From, const std::string & to );

  /// Move tuples, e.g. from "Events:Vertex:obj" to "Events:Vertices"
  void moveTuple ( const std::string & From, const std::string & to );

  /// Add the manipulations of another manipulator.
  /// (The other one takes precedence of this one).
  void add ( const UniversalManipulator & o );

  void manipulate ( Tuple & ) const;

  std::string name () const;
  UniversalManipulator * clone() const;

private:
  /// returns the "right" tuple name.
  std::string getTupleName ( const std::string & t ) const;
  std::string getColumnName ( const std::string & t ) const;
  void addTuple ( Tuple & t, const Tuple & orig, const std::string & prefix ) const;
  void addSimpleData ( Tuple &, const std::map < std::string, MultiType > & data,
      const std::string & prefix ) const;
  void addNestedData ( Tuple &, const std::map < std::string, Tuple * > & data,
      const std::string & prefix ) const;

private:
  std::set < std::string > theDroppedTuples;
  std::set < std::string > theDroppedColumns;
  mutable std::map < std::string, std::string > theRenamedColumns;
  mutable std::map < std::string, std::string > theRenamedTuples;
  mutable std::map < std::string, std::string > theMovedTuples;
  
};
}

#endif
