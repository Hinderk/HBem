
#ifndef __HBEM_BEMPLUGIN_H
#define __HBEM_BEMPLUGIN_H

#include <cstdint>
#include <stdlib.h>

#include "windows/dlfcn.h"
#include "plugin.h"





class BEM_Plugin : public Plugin {

  public:

    BEM_Plugin( const char *Name, int Flags = RTLD_NOW ) ;
   ~BEM_Plugin( void ) ;

} ;

#endif // __HBEM_BEMPLUGIN_H
