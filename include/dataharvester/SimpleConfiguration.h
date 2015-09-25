#ifndef _SimpleConfiguration_H_
#define _SimpleConfiguration_H_

#include <dataharvester/MultiType.h>
#include <string>
#include <map>

namespace dataharvester {
class SimpleConfiguration
{
    /**
     *  Simple implementation of CARF's SimpleConfiguration.
     */
public:
    static SimpleConfiguration* current();
    void status() const;
    void modify( const std::string & name, const MultiType & value );
    void setDefault( const std::string & name, const MultiType & value );
    MultiType value( const std::string & name );

private:
    SimpleConfiguration();
    std::map< std::string, MultiType > myMap;
    std::map< std::string, bool > myHas;
};
}

#endif
