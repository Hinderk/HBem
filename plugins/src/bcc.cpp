
#include <iostream>
#include <cstdint>

#include "bcc.h"
#include "setup.h"



extern "C" int32_t HBEM_Exit( void )
{
  return 0 ;
}



extern "C" int32_t HBEM_Init( const Service_t *Service )
{
  return Setup<BC_C>::Init( Service ) ;
}



int BC_C::Evaluate( PointData &p )
{
  p.valid_bc = p.material < 10 ;
  if ( p.valid_bc )
  {
    p.c0 = 1 ;
    p.c1 = 0 ;
    p.cr = p.midpoint.x * p.midpoint.x - p.midpoint.y * p.midpoint.y ;
  }
  return 0 ;
}
