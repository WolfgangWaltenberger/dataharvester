%define DOCSTRING
"Swig wrapper of the dataharvester. Enables transparent read/write access to
root, hdf, and Writer file formats from languages like python"
%enddef

%module(docstring=DOCSTRING) dataharvester
%{
#include <dataharvester/Writer.h>
#include <dataharvester/AbstractWriter.h>
#include <dataharvester/Reader.h>
#include <dataharvester/AbstractReader.h>
#include <dataharvester/Tuple.h>
#include <dataharvester/Timer.h>
#include <dataharvester/Manipulator.h>
#include <dataharvester/UniversalManipulator.h>
#include <dataharvester/SimpleConfigurable.h>
#include <dataharvester/SimpleConfiguration.h>
#include <dataharvester/CommentsExpresser.h>
#include <dataharvester/CommentsExpresser.h>
#include <dataharvester/CommentsCollector.h>
#include <dataharvester/TupleNameFilter.h>
#include <dataharvester/TupleRow.h>
#include <dataharvester/MultiType.h>
#include <dataharvester/Mode.h>
#include <dataharvester/HarvestingException.h>
#include <dataharvester/TerminateException.h>
#include <dataharvester/SystemWriter.h>

using namespace dataharvester;
%}

%feature("autodoc",1);
%include "std_string.i"
%include "std_map.i"
%include "std_vector.i"
%include "typemaps.i"
%include "exception.i"
%include "cpointer.i"

%exception {
    try {
        $action
    } catch (...) {
       //  SWIG_exception(SWIG_StopIteration, "It's over baby.");
        PyErr_SetString(PyExc_StopIteration,"It's over baby.");
        return NULL;
    }
}

%feature ("autodoc", "An AbstractReader is something like a read-only file handle.
In python it is an iterator object:
'for tuple in reader: print tuple'.
'reader.setFilter ( filter )' sets a filter.
'reader.setManipulator ( )' defines a manipulator that is used.
'reader[tuplename]' is the same as reader.setFilter ( TupleNameFilter ( tuplename ).
" ) AbstractReader;

%feature ("autodoc", "A Tuple is something like an 'object' with hierarchic
content in the Writer world. A Tuple consists of any number of TupleRows,
which is a heterogenous container.
tuple[rownr] returns the row number 'rownr'.
tuple[name] retrieves all occurences of name in tuple.
A Tuple is iterable:
'for row in tuple: print row'" ) Tuple;

%feature ("autodoc", "A TupleRow represents one line in a table (a Tuple). A TupleRow is iterable:
'for key in row: print key, row[key]'. Entries can be accessed directly: row[key]." ) TupleRow;

%feature ("autodoc", "A MultiType implements weak typing in C++. In python it is rather redundant.
It should be invisible to the python user.
" ) MultiType;

%rename(__getitem__)         *::operator[];
%rename(__getitem__)         *::operator[] const;
%rename(convertToDouble)     *::operator double;
%rename(convertToFloat)      *::operator float;
%rename(convertToString)     *::operator std::string;
%rename(convertToLong)       *::operator long;
%rename(convertToInt)        *::operator int;
%rename(convertToBool)       *::operator bool;
%rename(assign)              *::operator=;
%rename(streamOut)           operator<<;

%include <dataharvester/Writer.h>
%include <dataharvester/AbstractWriter.h>
%include <dataharvester/Reader.h>
%include <dataharvester/AbstractReader.h>
%include <dataharvester/Tuple.h>
%include <dataharvester/Timer.h>
%include <dataharvester/TupleRow.h>
%include <dataharvester/Filter.h>
%include <dataharvester/Manipulator.h>
%include <dataharvester/UniversalManipulator.h>
%include <dataharvester/CommentsExpresser.h>
%include <dataharvester/CommentsCollector.h>
%include <dataharvester/SimpleConfigurable.h>
%include <dataharvester/SimpleConfiguration.h>
%include <dataharvester/TupleNameFilter.h>
%include <dataharvester/MultiType.h>
%include <dataharvester/Mode.h>
%include <dataharvester/HarvestingException.h>
%include <dataharvester/TerminateException.h>
%include <dataharvester/SystemWriter.h>

%fragment("PySwigIterator");

namespace std {
    %template(DHMap) map < string, MultiType >;
    %template(DHVector) vector < MultiType >;
    %template(TupleRows) vector < TupleRow >;
    %template(DHNestedMap) map < string, Tuple >;
    %template(DHNestedPtrMap) map < string, Tuple * >;
    %template(DHStringVector) vector < string >;
    %template(DHDoubleVector) vector < double >;
    %template(DHLongVector) vector < long >;
}

using namespace dataharvester;

%extend dataharvester::MultiType {
    const char * __str__() {
        return self->asString().c_str();
    }
}

%pythoncode {
  def multiTypeToNativeType ( m ):
     """ Convert a MultiType into a 'native' python
         variable. """
     if not m.__class__ == MultiType: return m
     if ( m.isA() in [ "double", "float" ] ):
       return m.asDouble()
     if ( m.isA() in [ "int64", "int32" ] ):
       return m.asInt()
     if ( m.isA() == "bool" ):
       return m.asBool()
     if ( m.isA() == "string" ):
       return m.asString()
     if ( m.isA() in [ "int32vec", "int64vec" ] ):
       return list( m.asIntVector() )
     if ( m.isA() in [ "doublevec", "floatvec" ] ):
       return list( m.asDoubleVector() )
     return m

  def convertFile ( source, dest ):
      for i in Reader_file( source ):
          Writer_file( dest ).save(i)
      Writer_close()
}

%extend dataharvester::Tuple {
    %pythoncode {
     def __str__ ( self ): return self.contains()

     def getAttribute ( self, key ):
       """ of all tuple rows, collect all columns named 'key' """
       a=[]
       b=self.getAll ( key )
       if not b==None:
         for c in b:
           a.append ( multiTypeToNativeType ( c ) )
       return a

     def __getitem__ ( self, s ):
       if s.__class__==int:
         if self.getNumberOfRows() > s:
           return self.getRow(s)
         else:
           return None
       if self.hasValue ( s ):
         return self.getAttribute ( s )
       if self.getNumberOfRows()==0:
         return None
       if self.getNumberOfRows() == 1:
         if self.hasDescendantInFirstRow ( s ):
           return self.descendInFirstRow ( s )
         if self.hasValueInFirstRow ( s ):
           return self.getRow(0)[s]
       # more than one row
       if self.hasValueInFirstRow ( s ):
         return self.getAll ( s )
       return None

     def __setitem__ ( self, s, m ):
       mt=MultiType(m)
       if m.__class__ == type([]):
         mt=MultiType()
         for i in m:
           mt.addToVector ( i )
       self.add ( str(s), mt )
    }
}

%extend dataharvester::TupleRow {
    %pythoncode {
      def __str__( self ): return self.contains()

      def __getitem__ ( self, s ):
        if ( self.getSimpleData().count(s) ):
          return multiTypeToNativeType ( self.value(str(s)) )
        if ( self.getNestedData().count(s) ):
          return self.descend(s)
        return None


    }

}

%extend dataharvester::AbstractReader {
    %pythoncode {
        def __getitem__ ( self, s ):
          t=TupleNameFilter(s)
          self.setFilter(t)
          return self

        def __iter__ ( self ): return self

        def __str__ ( self ):
          return self.describe()

        def getAll ( self, column ):
          """ In all rows of all tuples retrieve all column values named 'column'.
              Use with care. """
          a=[]
          for i in self:
            tmp=i[column]
            for t in tmp:
              a.append(multiTypeToNativeType(t))
          self.reset()
          return a
    }
}

