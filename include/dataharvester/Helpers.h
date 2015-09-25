#ifndef Helpers_H
#define Helpers_H

#include <string>
#include <utility>

namespace dataharvester {
class Helpers {
public:
  /**
   *  create a backup of the file "name"
   *  \paramname copy true: make a real copy. false: just rename file.
   */
  static void createBackup ( const std::string & name, bool copy=false );

  /// standardised name for a backup.
  /// (Used by createBackup ).
  static std::string backupName ( const std::string & name );

  /**
   *  verbosity of the framework */
  static int verbose();

  /**
   *  turn hierarchic data into a flat design?
   */
  static bool flatData();

  /**
   *  set the verbosity of the framework */
  static void setVerbosity ( int );

  /**
   *  Check the name of the Tuple;
   *  __comments__ as a name is reserved!
   */
  static bool checkTupleName ( const std::string & );

  /**
   *  make it possible to implement a flag that turns off
   *  all persistency
   */
  static bool dontSave();

  /**
   *  shall the seeders ignore meta data?
   */
  static bool ignoreMetaData();

  /**
   *  maximum string length in a string field.
   */
  static int maxStringLength();

  /**
   *  split columnname into actual name
   *  and hint as to the length of the value.
   *  (only makes sense for std::strings).
   *  "surname@16" -> field "surname",
   *  field length = 16 characters.
   */
  static std::pair < std::string, int > extractLength ( std::string );

  /**
    * encode length info in column names?
    * valid only for std::strings.
    * ("surname",max 16 characters -> "surname@16")
    */
  static bool wantsLengthInfo();
};
}

#endif
