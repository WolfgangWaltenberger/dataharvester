#include "HarvestingConfiguration.h"
#include "Helpers.h"
#include "ConfigurationSetter.h"
#include "Writer.h"

int main()
{
  harvest::Helpers::setVerbosity ( 255 );
  std::string sink = "dhtp://debcms.shacknet.nu:39824/";
  harvest::Tuple m ( "date" );
  m["age"]=24.;
  m["name"]="Pavi";
  // ConfigurationSetter s ( "
  harvest::Writer::file ( sink + "f1.root" ).save ( m );
  harvest::Writer::file ( sink + "f2.root" ).save ( m );
  harvest::Writer::close();
}
