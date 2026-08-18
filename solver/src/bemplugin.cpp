
#include "defines.h"
#include "bemplugin.h"




BEM_Plugin::BEM_Plugin( const char *Name, int Flags ) : Plugin()
{
  if ( Name )
  {
    State = HBEM_FAILED_TO_LOAD_PLUGIN ;
    SharedObject = dlopen( Name, Flags ) ;
    if ( SharedObject )
    {
      Init = (Load_t) dlsym( SharedObject, "HBEM_Init" ) ;
      Exit = (Unload_t) dlsym( SharedObject, "HBEM_Exit" ) ;
      State = Init && Exit ? 0 : HBEM_FAILED_TO_RESOLVE_SYMBOLS ;
    }
  }
}



BEM_Plugin::~BEM_Plugin( void )
{
  if ( SharedObject )  dlclose( SharedObject ) ;
}
