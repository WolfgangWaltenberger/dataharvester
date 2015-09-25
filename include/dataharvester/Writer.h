#ifndef dataharvester_Writer_H
#define dataharvester_Writer_H

#include <string>
#include <vector>
#include <dataharvester/AbstractWriter.h>
#include <dataharvester/Mode.h>
#include <dataharvester/MultiType.h>

// The following include is for the user's convenience only.
#include <dataharvester/Tuple.h>

namespace dataharvester {
class Writer {
public:
  /**
   *  \class Writer
   *  The user frontend for writing dataharvester files (root, hdf, whatever).
   *
   *  Example:
   *  using namespace dataharvester;
   *  Tuple data("People");
   *  data["Name"]= "Donald Duck";
   *  data.describe ( "The Name of the person", "Name" );
   *  data["Age"] = 16.;
   *  data["married"] = false;
   *
   *  Writer::file("myfile.root") << data;
   *
   *  The text within brackets (like this text here) is considered a
   *  description of the ntuple or leaf. All descriptions are saved in special
   *  ntuples, as well.
   *
   *  ... and then at the end:
   *  Writer::file("myfile.root").close();
   *  or:
   *  Writer::close(); to close all files at once.
   *
   *  There are two modes of operation for the Writer:
   *  Recreate and Update. The mode is chosen via a configurable:
   *  Mode:FileMode
   */

  /**
   *   This is the only way to access the only instantiation.
   */
  static AbstractWriter & file ( const std::string & s,
      Mode::FileMode = Mode::Recreate );

  /**
   *  Writes all unwritten data of all Writers. This needs
   *  to be called _before_ the destruction sequence of static
   *  objects begins! Aftwards nothing can be written to the files 
   *  anymore.
   */
  static void close();
  static void close( const std::string & filename );

  /**
   *  returns a list of all currently open dataharvesters
   */
  static std::vector < std::string > listWriters();
private:
  Writer();
  ~Writer();
};
}

#endif
