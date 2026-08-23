
#include "vector.h"

using namespace HBEM ;



Vector::Vector( void ) :

  f( NULL ) ,
  size( 0 ) {}



Vector::~Vector( void ) { if ( f )  delete[] f ; }



Vector::Vector( uint64_t length ) : f( NULL ), size( length )
{
  if ( length > 0 )
  {
    f = new double[ length ] ;
    for ( int64_t i = 0 ; i < length ; i ++ )  f[ i ] = 0.0 ;
  }
}



void Vector::Resize( uint64_t length )
{
  if ( size > 0 )  delete[] f ;
  size = length ;
  f = NULL ;
  if ( size > 0 )
  {
    f = new double[ size ] ;
    for ( int64_t i = 0 ; i < size ; i ++ )  f[ i ] = 0.0 ;
  }
}



double Vector::operator()( uint64_t index ) const
{
  return f[ index ] ;
}



double& Vector::operator()( uint64_t index )
{
  return f[ index ] ;
}
