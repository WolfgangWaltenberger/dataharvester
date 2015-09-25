#include <dataharvester/HarvestingConfiguration.h>
#include "DhfHelpers.h"

using namespace dataharvester;

MultiType::Type DhfHelpers::toType ( char c )
{
  switch (c)
  {
    case 's':
      { return MultiType::kString; break; }
    case 'i':
      { return MultiType::kInt32; break; }
    case 'I':
      { return MultiType::kInt64; break; }
    case 'd':
      { return MultiType::kDouble32; break; }
    case 'D':
      { return MultiType::kDouble64; break; }
    case 'b':
      { return MultiType::kBool; break; }
    case 'v':
      { return MultiType::kDouble32Vec; break; }
    case 'V':
      { return MultiType::kDouble64Vec; break; }
    case 'f':
      { return MultiType::kInt32Vec; break; }
    case 'F':
      { return MultiType::kInt64Vec; break; }
    case '?':
    default:
      return MultiType::kNone;
  }
}

const char DhfHelpers::newline='\n';
const char DhfHelpers::separator=':'; // '\2';
const char DhfHelpers::comma=','; // '\1'; 
const char DhfHelpers::prototype='P';
const char DhfHelpers::is_prototype='p';
const char DhfHelpers::data='D';

char DhfHelpers::toChar ( MultiType::Type m )
{
  switch (m)
  {
    case MultiType::kString:
      { return 's'; break; }
    case MultiType::kInt32:
      { return 'i'; break; }
    case MultiType::kInt64:
      { return 'I'; break; }
    case MultiType::kDouble32:
      { return 'd'; break; }
    case MultiType::kDouble64:
      { return 'D'; break; }
    case MultiType::kBool:
      { return 'b'; break; }
    case MultiType::kDouble32Vec:
      { return 'v'; break; }
    case MultiType::kDouble64Vec:
      { return 'V'; break; }
    case MultiType::kInt32Vec:
      { return 'f'; break; }
    case MultiType::kInt64Vec:
      { return 'F'; break; }
    case MultiType::kNone:
    default:
      return '?';
  }
}
