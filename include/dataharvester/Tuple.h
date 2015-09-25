#ifndef Tuple_H
#define Tuple_H

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <dataharvester/MultiType.h>
#include <dataharvester/TupleRow.h>

namespace dataharvester {
class Tuple {
  /**
   * \class Tuple
   * A class to take care of hierarchical data buffering, providing means
   * to flush the contents recursively.
   * A Tuple consists of zero or more TupleRows, which, in turn
   * consist of "simple" data ( key=value ) plus
   * "nested" data ( key=Tuple ).
   **/

  public:
    Tuple();
    Tuple( const char *name, const char *comment="" );
    Tuple( const std::string & name, const std::string & comment="" );
    ~Tuple();

    //* Add a row.
    void add ( const TupleRow & );

    //* Set an entry in the current row;
    //* (tuple["id"]=23)
    //* also, get current value
    //* int id = tuple["id"]
    MultiType & operator[] ( const char *);
    MultiType & operator[] ( const std::string &);

    // Another way to set a variable, one that needs no
    // operators.
    void add ( const std::string & name, const MultiType & value );

    //* From all tuple rows, collect all occurences of "path".
    std::vector < MultiType > getAll ( const char * path ) ;

    //* Retrieve first occurence of "path"
    MultiType getFirst ( const char * path );

    // Void addRow ( std::string, std::map < std::string, MultiType > & mp );
    std::string  getName() const;
    std::string  getDescription() const;

    /// Get the description of a column.
    std::string  getDescription ( const std::string & column ) const;

    void clear(); //< Clears _all_ tuple rows!

    //* Do we have this variable in the current row?
    bool hasValue ( const char * ) const;

    //* Fill up, start a new row,
    //* or fill up subbranch ( "path" )
    bool fill( const char * path = "", bool warn_empty_root=true );
    bool fill( const std::string & path, bool warn_empty_root=true );

    //* Change the description string of the Tuple (or of a substructure)
    bool describe( const char * descr, const char *path="" );
    bool describe( const std::string & descr, const std::string & path="" );

    //* Briefly describe this tuple
    //* (name and number of tuple rows)
    std::string contains() const;

    //* Recursively dump the contents to stdout
    std::string toString ( std::string prefix="", unsigned int indent=4, bool type = true );
    void dump( std::string prefix="", unsigned int indent=4, bool type = true );

    //* Returns all tuple rows
    std::vector < TupleRow > getTupleRows() const;

    //* Return row with a certain index
    const TupleRow & getRow ( unsigned idx ) const;
    
    //* Return row, non-const.
    TupleRow & getMutableRow ( unsigned idx );

    const TupleRow & getCurrentRow() const;

    //* Return the number of tuple rows
    unsigned getNumberOfRows() const;
    
    //* Determine if there is an element with that name among 
    //* the descendants of the first row.
    //* Checks if n_rows > 0.
    //* Convenience function.
    bool hasDescendantInFirstRow( const std::string & name ) const;

    //* Return the Tuple "name" of the first row.
    //* Checks if n_rows > 0.
    //* Convenience function.
    Tuple & descendInFirstRow( const std::string & name );

    //* Do we have this value in the first row?
    //* Checks if n_rows > 0.
    //* Convenience function.
    bool hasValueInFirstRow ( const std::string & name ) const;

    //* Python convenience, making Tuple an iterable container
    const Tuple & __iter__() const;
    const TupleRow & next() const;

    /// Setting name and description.
    void setName ( const std::string & );
    void setDescription ( const std::string & );

  private:
    // FIXME: make a list out of this ?
    //  std::vector is doing lots of pre-alloc, +more overhead than necessary ?
    std::vector< TupleRow >   tupRows_; // tuple rows
    TupleRow      curRow_; // current row
    std::string  tname_; // tuple name
    std::string  tdescr_; // tuple description
    mutable unsigned rowctr_; // row counter
};
}

std::ostream & operator << ( std::ostream & s, dataharvester::Tuple & );

#endif // Tuple_H
