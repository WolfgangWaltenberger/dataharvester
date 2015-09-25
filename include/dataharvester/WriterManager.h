#ifndef WriterManager_H
#define WriterManager_H

#include <string>
#include <map>

namespace dataharvester {
class AbstractWriter;
class StreamableWriter;
class WriterManager {
  /**
   *  Class that manages the file formats that
   *  are supported by the Writer
   */
public:
  static const int FileFormat=0;
  static const int NetworkProtocol=1;
  static WriterManager * self ();

  /**
   *  Registers a suffix or protocol, depending on type
   */
  void registers ( int type, AbstractWriter *, const std::string & );

  /**
   *  Returns the Writer according to a suffix
   */
  AbstractWriter * writer ( const std::string & filetype );
  /**
   *  Returns the Writer according to a protocol
   */
  AbstractWriter * netWriter ( const std::string & protocol );

  /**
   *  Returns a streamable writer, 0 if not available
   */
  StreamableWriter * streamableWriter ( const std::string & filetype );

  /**
   *  Returns the complete std::map
   */
  // std::map < std::string, AbstractWriter * > supportedWriters();

private:
  WriterManager();
  ~WriterManager();
  /**
   *  This registers a suffix
   */
  void registerFileType ( AbstractWriter *,
                          const std::string & filetype );
  /**
   *  This registers a transfer protocol
   */
  void registerProtocol ( AbstractWriter *,
                          const std::string & protocol );


private:
  mutable std::map < std::string, AbstractWriter * > theWriter;
  mutable std::map < std::string, AbstractWriter * > theNetWriter;
};
}

#endif
