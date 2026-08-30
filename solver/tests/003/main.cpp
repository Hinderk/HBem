
#include <iostream>
#include <cstring>
#include <fstream>

#include "container.h"
#include "defines.h"
#include "meshdata.h"
#include "domaindata.h"



int main( int argc, const char *argv[] )
{
  using namespace std ;
  using namespace HBEM ;
  if ( argc < 3 )  return -1 ;
  DomainData DD ;
  DD.ChangeLogLevel( 16 ) ;
  DD.Load( argv[2] ) ;
  DD.Read( argv[1] ) ;
  MeshData M0 ;
  int state = DD.CreateMesh( M0, 0.1 ) ;
  int expected[] = { 0, 2, -6, -2 , -4, -1, -1, -1, 1, -6, 3 } ;
  const Point2D Q[] = { { 0, 0 }, { 2.5, 2.5 }, { 2, 2 }, { 4, 4 },
                        { -3, -3 }, { -2, 0 }, { 1, -4 }, { 4, -1 },
                        { -3, 0 }, { -2, -2 }, { -2.5, -2.5 } } ;
  Position Location[ 16 ] ;
  printf( "\n\n" ) ;
  int i = 0 ;
  for ( const auto p : Q )
  {
    int result = M0.Query( p, Location ) ;
    printf( "\n Point: (%f,%f)", p.x, p.y ) ;
    printf( "\n Return Value: %i\n", result ) ;
    state += ( result != expected[ i ++ ] ) ;
  }
  printf( "\n Test Result: %s ...\n", state ? "NOK" : "OK" ) ;
  return state ;
}
