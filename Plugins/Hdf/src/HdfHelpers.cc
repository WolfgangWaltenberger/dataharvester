#include <dataharvester/HarvestingConfiguration.h>
#include "HdfHelpers.h"

using namespace H5;
using namespace std;
using namespace dataharvester;

int HdfHelpers::sizeOf ( const MultiType & m, const string & name )
{
  switch ( m.isType() )
  {
    case MultiType::kDouble32:
      return sizeof(float);
      // return sizeof(double); // FIXME is this ok?
    case MultiType::kDouble64:
      return sizeof(double);
    case MultiType::kInt64:
      return sizeof(int);
    case MultiType::kInt32:
      return sizeof(int);
    case MultiType::kBool:
      return sizeof(int);
    default:
      cout << "[HdfHelpers] error: unknown MultiType ``" << m.isA() << "'' (value="
           << m.asString() << ", name=" << name << ")" << endl;
      return sizeof(int);
  };
}

void HdfHelpers::insertRightType ( 
    const CompType * ct, string column, size_t offset, const MultiType & m )
{
  // FIXME I dont know how to correctly use these fucking PredTypes
  // Ref Counting works in a weird way for them
  // So I make sure I only have this volatile copy
  // cout << "insert " << column << " at " << offset;
  switch ( m.isType() )
  {
    case MultiType::kDouble32:
    {
      ct->insertMember ( column, offset, PredType::NATIVE_FLOAT );
      // cout << "(float)";
      break;
    };
    case MultiType::kDouble64:
    {
      ct->insertMember ( column, offset, PredType::NATIVE_DOUBLE );
      // cout << "(double)";
      break;
    };
    case MultiType::kInt64:
    {
      ct->insertMember ( column, offset, PredType::NATIVE_INT );
      // cout << "(int)";
      break;
    };
    case MultiType::kInt32:
    {
      ct->insertMember ( column, offset, PredType::NATIVE_INT );
      // cout << "(int)";
      break;
    };
    case MultiType::kBool:
    {
      ct->insertMember ( column, offset, PredType::NATIVE_INT );
      break;
    };
    // cout << endl;
    case MultiType::kString:
    default:
    {
      cout << "[HdfWriter] gromph. Dunno how to handle strings here :-(" << endl;
      // returning DataType instead of PredType doesnt work,
      // either
      ct->insertMember ( column, offset, PredType::C_S1 );
      break;
      // return PredType::C_S1;
    };
  };
}

