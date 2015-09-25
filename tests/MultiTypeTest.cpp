#include <dataharvester/HarvestingConfiguration.h>
#include <dataharvester/Cout.h>
#include <dataharvester/Helpers.h>
#include <dataharvester/MultiType.h>
#include <cassert>
#include <cmath> // M_PI

using namespace std;
using namespace dataharvester;

namespace {
  using namespace Cout;

  void ok()
  {
    cout << " ... " << info() << "ok" << reset() << endl;
  }

  void stringtest()
  {
    cout << "Stringtest: ";
    string test="hallo";
    MultiType mt(test);
    assert ( mt.isA() == "string" );
    assert ( mt.asString() == "hallo" );
    assert ( mt == "hallo" );

    MultiType mt2;
    mt2=test;
    assert ( mt2.isA() == "string" );
    assert ( mt2.asString() == "hallo" );
    assert ( mt2 == "hallo" );
    cout << "    " << test << " -> " << mt << " -> " << mt2;
    ok();
  }

  void doubletest()
  {
    cout << "Doubletest: ";
    double d1=M_PI;
    MultiType mt3(d1);
    assert ( mt3.isA() == "double" );
    assert ( mt3.asDouble() == d1 );
    assert ( (double) mt3 == d1 );

    MultiType mt4;
    mt4=d1;
    assert ( mt4.isA() == "double" );
    assert ( mt4.asDouble() == d1 );
    assert ( (double) mt4 == d1 );
    cout << "    " << d1 << " -> " << mt3 << " -> " << mt4;
    ok();
  }

  void inttest()
  {
    cout << "Inttest:    ";
    int orig = 1234567890;
    MultiType mt5(orig);
    cout << "    " << orig << " -> " << mt5 << flush;
    assert ( mt5.isA() == "int" );
    assert ( mt5.asLong() == orig );
    assert ( (int) mt5 == orig );


    MultiType mt6;
    mt6=-orig;
    assert ( mt6.isA() == "int" );
    assert ( mt6.asLong() == -orig );
    assert ( (int) mt6 == -orig );

    cout << "    " << mt6 << " -> " << -orig;
    ok();
  }

  void booltest()
  {
    cout << "Booltest:   ";
    bool orig=true;
    MultiType mt7(orig);
    assert ( mt7.isA() == "bool" );
    assert ( mt7.asBool() == orig );
    cout << "    " << mt7 << " -> " << orig;
    ok();
  }

  void crosstest()
  {
    cout << "Crosstest:    ";
    string orig=".31415926e1";
    MultiType mt(orig);
    assert ( mt.asDouble() == 3.1415926 );
    MultiType mt2 ( mt.asDouble() );
    assert ( mt.asDouble() == mt2.asDouble() );
    cout << "    " << orig << " -> " << mt << " -> " << mt.asDouble() << " -> " << mt2;

    cout << endl;
    int iorig=23;
    mt = iorig;
    assert ( mt.asInt() == iorig );
    mt2 = mt.asInt();
    assert ( mt2.asInt() == mt.asInt() );
    cout << "    " << iorig << " -> " << mt << " -> " << mt.asInt() << " -> " << mt2;

    cout << endl;
    bool borig=23;
    mt = borig;
    assert ( mt.asBool() == borig );
    mt2 = mt.asBool();
    assert ( mt2.asBool() == mt.asBool() );
    cout << "    " << borig << " -> " << mt << " -> " << mt.asBool() << " -> " << mt2;
    ok();
  }
}

int main()
{
  /*
  string s="@16";
  pair < string, int > t = Helpers::extractLength ( s );
  cout << s << ": " << t.first << ", " << t.second << endl;
  */
  Cout::useColors( true );
  cout << "Testing MultiType" << endl
       << "=================" << endl
       << endl;

  stringtest();
  doubletest();
  inttest();
  booltest();
  crosstest();
}
