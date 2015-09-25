#ifndef CompositeReader_H
#define CompositeReader_H

#include <string>
#include <vector>
#include <map>
#include <dataharvester/AbstractReader.h>

namespace dataharvester {
class CompositeReader : public AbstractReader {
public:
  /**
   *  \class CompositeReader
   *  A data sink multiplexer.
   */
  CompositeReader ( const std::vector < AbstractReader * > & );

  void add ( AbstractReader * );

  virtual Tuple next ( );
  virtual void reset();
  virtual void setFilter ( const Filter & );
  virtual std::string filename() const;
  virtual int getNumberOfEntries() const;

  virtual CompositeReader * clone ( const std::string & filename ) const;

  /**
   *  Destructor is also responsible for closing the file
   */
  virtual ~CompositeReader();
private:
  std::vector < AbstractReader * > theSinks;
  std::vector < AbstractReader * >::const_iterator thePtr;
};
}

#endif
