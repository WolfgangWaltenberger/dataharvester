#include <dataharvester/SimpleConfiguration.h>
#include <iomanip>
#include <iostream>

using namespace dataharvester;
using namespace std;

SimpleConfiguration::SimpleConfiguration()
{}

SimpleConfiguration* SimpleConfiguration::current()
{
    static SimpleConfiguration singleton;
    return &singleton;
}

void SimpleConfiguration::status() const
{
    for ( map < string, MultiType >::const_iterator i = myMap.begin(); 
          i != myMap.end(); ++i )
    {
        cout << setw(40) << i->first << " = " << i->second.asString() << endl;
    };
}

void SimpleConfiguration::modify( const string & name, const MultiType & value )
{
    myMap[name] = value;
    myHas[name] = true;
}

// setDefault is like modify.
// only that it doesnt override already set values.
void SimpleConfiguration::setDefault( const string & name, const MultiType & value )
{
    if ( !myHas[name] )
    {
        myMap[name] = value;
        myHas[name] = true;
    };
}

MultiType SimpleConfiguration::value( const string & name )
{
    return myMap[name];
}
