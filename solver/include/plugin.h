
#ifndef __PLUGIN_H
#define __PLUGIN_H

#include <cstdint>
#include <stdlib.h>

#ifndef RTLD_NOW
  #define RTLD_NOW  0x001
#endif



extern "C" {


  struct Service_t ;



  typedef struct Setup_t {

    const char              *Type ;
    int32_t                  State ;
    const struct Service_t  *Service ;

  } Setup_t ;



  typedef struct PluginVersion_t {

    bool operator<( const PluginVersion_t pv ) const
    {
      return Major < pv.Major ? true  :
             Major > pv.Major ? false :
             Minor < pv.Minor ;
    }

    uint16_t Major ;
    uint16_t Minor ;

  } PluginVersion_t ;




  typedef struct Plugin_t {

    typedef void* (*Create_t)( Setup_t * ) ;
    typedef void  (*Destroy_t)( const void *, Setup_t * ) ;

    PluginVersion_t  Version ;
    Create_t         Constructor ;
    Destroy_t        Destructor ;

  } Plugin_t ;



  typedef int32_t (*Register_t)( void *, const char *, const Plugin_t * ) ;
  typedef int32_t (*ServiceRequest_t)( void *, const char *, void * ) ;



  typedef struct Service_t {

    PluginVersion_t   Version ;
    void             *Registrar ;
    Register_t        Register ;
    ServiceRequest_t  Request ;

  } Service_t ;



  typedef int32_t (*Load_t)( const Service_t * ) ;
  typedef int32_t (*Unload_t)( void ) ;

}


class Plugin {

  public:

    Plugin() : SharedObject(NULL), Init(NULL), Exit(NULL), State(-1) {}

   ~Plugin() {}

    int init( const Service_t *S ) { return State ? State : Init(S) ; }
    int exit( void ) { return State ? State : Exit() ; }
    int state( void ) const { return State ; }

  protected:

    void *SharedObject ;

    Load_t Init ;
    Unload_t Exit ;

    int State ;

} ;

#endif // __PLUGIN_H
