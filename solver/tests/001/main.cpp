
#include <iostream>
#include <cstring>
#include <fstream>

#include "defines.h"
#include "domaindata.h"



int main( int argc, const char *argv[] )
{
  using namespace std ;
  if ( argc < 2 )  return -1 ;
  DomainData DD ;
  int state = DD.Read( argv[1] ) ;
  printf( "\n Return value reads: %i\n", state ) ;
  return state ;
}
