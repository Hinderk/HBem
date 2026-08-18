
#ifdef _WIN32

#include "windows/dlfcn.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



extern "C" {


  static struct
  {
    long code ;
    const char *source ;
  }

  LatestError = { 0, NULL } ;



  void* dlopen( const char *filename, int flags )
  {
    HINSTANCE handle ;
    handle = LoadLibrary( filename ) ;
    if ( handle == NULL )
    {
      LatestError.code = GetLastError() ;
      LatestError.source = "dlopen" ;
    }
    return handle ;
  }


  int dlclose( void *sharedobject )
  {
    HINSTANCE handle = (HINSTANCE) sharedobject ;
    if ( FreeLibrary( handle ) )
      return 0 ;
    else
    {
      LatestError.code = GetLastError() ;
      LatestError.source = "dlclose" ;
    }
    return -1 ;
  }


  void* dlsym( void *sharedobject, const char *symbolname )
  {
    HINSTANCE handle = (HINSTANCE) sharedobject ;
    FARPROC symbol ;
    symbol = GetProcAddress( handle, symbolname ) ;
    if ( symbol )  return (void*) symbol ;
    LatestError.code = GetLastError() ;
    LatestError.source = "dlsym" ;
    return NULL ;
  }


  const char* dlerror( void )
  {
    static char buffer[ 128 ] ;
    if ( LatestError.code )
    {
      snprintf( buffer, 128, "%s error nr. %ld",
                LatestError.source, LatestError.code ) ;
      LatestError = { 0, NULL } ;
      return buffer ;
    }
    return NULL ;
  }

}

#endif // _WIN32
