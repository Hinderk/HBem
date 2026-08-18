
#ifndef __HBEM_SETUP_H
#define __HBEM_SETUP_H

#include "plugin.h"
#include "defines.h"

#include <new>
#include <string.h>



template <class BEM_Plugin_t> class Setup {

  public:

    static int32_t Init( const Service_t *Service )
    {
      Plugin_t BEM ;
      BEM.Version = BEM_Plugin_t::Version ;
      if ( Service -> Version < BEM.Version )
      {
        BEM.Constructor = Setup::Create ;
        BEM.Destructor = Setup::Discard ;
        return Service ->
          Register( Service -> Registrar, BEM_Plugin_t::Type, &BEM ) ;
      }
      return HBEM_INCOMPATIBLE_PLUGIN_VERSION ;
    }

  private:

    static void* Create( Setup_t *Data )
    {
      try {
        BEM_Plugin_t *Handle = NULL ;
        Data -> State = 0 ;
        if ( strcmp( Data -> Type, BEM_Plugin_t::Type ) )
          Data -> State = HBEM_UNKNOWN_OBJECT_REQUESTED ;
        else
          Handle = new BEM_Plugin_t ;
        return Handle ;
      }
      catch ( const std::bad_alloc &e ) {
        Data -> State = HBEM_OBJECT_ALLOCATION_FAILED ;
      }
      return NULL ;
    }

    static void Discard( const void *Handle, Setup_t * )
    {
      delete (BEM_Plugin_t*) Handle ;
    }

} ;

#endif // __HBEM_SETUP_H
