#include "Utilities/Configuration/interface/Architecture.h"
#include "Vertex/DataHarvesting/interface/MultiType.h"
#include "Utilities/UI/interface/SimpleConfiguration.h"
// #include "Utilities/GenUtil/interface/CapriInit.h"
#include <cassert>

int main()
{
  // initialise this configurable business
  SimpleConfiguration conf(".",".orcarc",true);
//  Capri::Init cinit;

  cout << "Testing MultiType" << endl;
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
  
  double d1=975.23e12;
  MultiType mt3(d1);
  assert ( mt3.isA() == "double" );
  assert ( mt3.asDouble() == d1 );
  assert ( (double) mt3 == d1 );

  MultiType mt4;
  mt4=d1;
  assert ( mt4.isA() == "double" );
  assert ( mt4.asDouble() == d1 );
  assert ( (double) mt4 == d1 );

  MultiType mt5(12);
  assert ( mt5.isA() == "int" );
  assert ( mt5.asLong() == 12 );
  assert ( (int) mt5 == 12 );

  MultiType mt6;
  mt6=-12;
  assert ( mt6.isA() == "int" );
  assert ( mt6.asLong() == -12 );
  assert ( (int) mt6 == -12 );

  MultiType mt7(true);
  assert ( mt7.isA() == "bool" );
  assert ( mt7.asBool() == true );
  
  cout << "MultiType tested" << endl;
}
