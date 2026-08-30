
#include <iostream>
#include <cstring>
#include <math.h>




int Search( int pos, int *panel, int size )
{
  if ( pos < panel[ 0 ] )  return 0 ;
  int ka = 0 ;
  int ko = size - 1 ;
  int loop = 0 ;
  while ( ka < ko )
  {
    printf( "\n# -- Loop: %i - [% 4i,% 4i]", loop, ka, ko ) ;
    int kn = ( 1 + ka + ko ) >> 1 ;
    if ( pos < panel[ kn ] )
      ko = kn - 1 ;
    else
      ka = kn ;
    loop ++ ;
  }
  return ka ;
}



int main( int argc, const char *argv[] )
{
  int Values[ 1024 ] ;
  for ( int n = 0 ; n < 1024 ; n ++ )  Values[ n ] = 10 * n + 10 ;
  int pick[] = { 2, 24, 40, 55, 5129, 10251 } ;
  int state = -1 ;
  for ( int i : pick )
  {
    printf( "\n# Searching for: %i", i ) ;
    int res = Search( i, Values, 1024 ) ;
    printf( "\n# Lower bound at: %i", res ) ;
    printf( "\n# Sanity Check: %i", Values[ res ] ) ;
    state += i < Values[ res ] ;
  }
  printf( "\n## %i\n", state ) ;
  return state ;
}
