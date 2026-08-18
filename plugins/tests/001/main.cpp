
#include "domaindata.h"



int main( void )
{
  DomainData BEM ;
  PointData p0 ;
  BEM.ChangeLogLevel( 32 ) ;
  BEM.Load( "../../../modules" ) ;
  return 0 ;
}
