
#ifndef __DLFCN_H
#define __DLFCN_H

#ifdef _WIN32

  #define RTLD_LOCAL  0x000
  #define RTLD_LAZY   0x000
  #define RTLD_NOW    0x001
  #define RTLD_GLOBAL 0x100


  extern "C" {

    void* dlopen( const char *filename, int flags ) ;
    void* dlsym( void *sharedobject, const char *symbolname ) ;

    int dlclose( void *sharedobject ) ;

    const char* dlerror( void ) ;

  }

#else
  #include <dlfcn.h>
#endif                    // _WIN32

#endif // __DLFCN_H
