#ifndef TupleRow_H
#define TupleRow_H

#include <string>
#include <iostream>
#include <map>
#include <dataharvester/MultiType.h>

namespace dataharvester {
class Tuple;

class TupleRow {
  /**
   * \class TupleRow
   * A TupleRow is one "row" of a Tuple.
   * It contains two types of data:
   * "simple" data (key=value)
   * and
   * "nested" data (key=Tuple)
   **/

  public:
    TupleRow();
    TupleRow ( const std::string & name, const std::string & description );
    TupleRow( const TupleRow & orig);
    ~TupleRow();
    
    TupleRow & operator= ( const TupleRow & );
    // useful ?
    //TupleRow &    operator= ( const Tuple & );
    //MultiType &     operator= ( MultiType & );
    //MultiType &     operator[] ( const char *);

    //* Return the corresponding MultiType in operator[] syntax
    MultiType & operator[] ( const std::string & name );

    //* Determine if there exists a MultiType with that name
    bool hasValue( const std::string & name) const;

    //* Return the corresponding MultiType
    MultiType & value( const std::string & name );

    //* Determine if there is an element with that name among descendants
    bool hasDescendant( const std::string & name ) const;

    //* Return the corresponding Tuple
    Tuple & descend( const std::string & name ) const;

    //* Answer if there is any content, either as simple or nested data
    bool isEmpty() const;  

    //* Clear both std::maps (i.e. both simple and nested data), making isEmpty()==true
    void clear(); 

    /// Dump the content of the TupleRow.
    void dump( std::string prefix="", unsigned int indent=0, bool type = true );
    std::string toString( std::string prefix, unsigned int indent, bool type = true );

    /// Describe the content of the TupleRow.
    std::string contains() const;

    /// Getset description of TupleRow.
    std::string getDescription () const;
    void setDescription ( const std::string & );

    /// Get/set name of TupleRow.
    std::string getName() const;
    void setName ( const std::string & );

    /// Get description of column.
    std::string getDescription ( const std::string & ) const;

    /// Set the description of column.
    bool describe ( const std::string & description, 
                    const std::string & path );

    /// Retrieve the complete maps.
    std::map < std::string, MultiType > getSimpleData() const;
    std::map < std::string, Tuple * > getNestedData() const;

    /// Delete data. The Pointers are _not_ checked for validity!!
    void eraseFromSimple ( const std::string & );
    void eraseFromNested ( const std::string & );


    /// Rename data. The Pointers are _not_ checked for validity!!
    void renameSimple ( const std::string & From, const std::string & to );
    void renameNested ( const std::string & From, const std::string & to );

    /// Python convenience, making this iterable
    const TupleRow & __iter__() const;
    const std::string & next() const;

    /// Erase from simple data.
    void erase ( const std::string & s );

private:
    std::string name_;
    std::string description_;
    std::map< std::string, MultiType > simpleData_;
    mutable std::map< std::string, Tuple * >   nestedData_;
    mutable std::map< std::string, MultiType >::const_iterator simplePtr_;
    mutable std::map< std::string, Tuple * >::const_iterator nestedPtr_;
    mutable std::map < std::string, std::string > columnDescrs_; // column descriptions
};
}

std::ostream & operator << ( std::ostream & s, dataharvester::TupleRow & );

#endif // TupleRow_H
