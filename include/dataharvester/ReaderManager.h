#ifndef ReaderManager_H
#define ReaderManager_H

#include <string>
#include <map>

namespace dataharvester {
class AbstractReader;
class StreamableReader;

class ReaderManager {
  /**
   *  Class that manages the file formats that
   *  are supported by the Reader
   */
public:
  static ReaderManager * self ();

  /**
   *  This registers a suffix
   */
  void registerFileType ( AbstractReader *,
                          const std::string & filetype );

  /**
   *  Returns the Reader according to a suffix
   */
  AbstractReader * reader ( const std::string & filetype );

  /**
   *  Returns a streamable Reader according to a suffix,
   *  NULL if not available ( or not streamable )
   */
  StreamableReader * streamableReader ( const std::string & filetype );


  /**
   *  Returns the complete std::map
   */
  std::map < std::string, AbstractReader * > supportedReaders();

private:
  ReaderManager();
  ~ReaderManager();

private:
  mutable std::map < std::string, AbstractReader * > theReader;
};
}

#endif
