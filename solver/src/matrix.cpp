
#include <stdlib.h>
#include <math.h>

#include <suitesparse/umfpack.h>

#include "defines.h"
#include "vector.h"
#include "matrix.h"

using namespace HBEM ;



Matrix::Matrix( void ) :

  Ax( NULL ) ,
  Ai( NULL ) ,
  Ap( NULL ) ,
  cols( 0 ) {}




Matrix::~Matrix( void )
{
  if ( Ax )  delete[] Ax ;
  if ( Ai )  delete[] Ai ;
  if ( Ap )  delete[] Ap ;
}



Matrix::Matrix( uint64_t size ) : Ax( NULL ), Ai( NULL )
{
  Create( size ) ;
}



void Matrix::Create( uint64_t size )
{
  cols = size ;
  Ap = new int64_t[ size + 1 ] ;
  Ap[ size ] = size * size ;
  if ( size > 0 )
  {
    Ax = new double[ size * size ] ;
    Ai = new int64_t[ size * size ] ;
    uint64_t n = 0 ;
    for ( uint64_t i = 0 ; i < size ; i ++ )
    {
      Ap[ i ] = n ;
      for ( uint64_t j = 0 ; j < size ; j ++ )
      {
        Ax[ n ] = 0.0 ;
        Ai[ n ] = j ;
        n ++ ;
      }
    }
  }
}



void Matrix::Resize( uint64_t size )
{
  if ( cols > 0 )
  {
    delete[] Ax ;
    delete[] Ai ;
    Ax = NULL ;
    Ai = NULL ;
  }
  delete[] Ap ;
  Create( size ) ;
}


// Do not use the following operator on compressed matrices!

double& Matrix::operator()( uint64_t row, uint64_t col )
{
  return Ax[ col * cols + row ] ;
}



int Matrix::Compress( double threshold )
{
  uint64_t read_cursor = 0 ;
  uint64_t write_cursor = 0 ;
  for ( uint64_t c = 0 ; c < cols ; c ++ )
  {
    Ap[ c ] = write_cursor ;
    for ( uint64_t r = 0 ; r < cols ; r ++ )
    {
      const double A = Ax[ read_cursor ++ ] ;
      if ( threshold < fabs( A ) )
      {
        Ai[ write_cursor ] = r ;
        Ax[ write_cursor ++ ] = A ;
      }
    }
  }
  Ap[ cols ] = write_cursor ;
  if ( cols > 0 )
  {
    size_t newsize = write_cursor * sizeof( double ) ;
    void *handle = realloc( Ax, newsize ) ;
    if ( handle )
      Ax = (double*) handle ;
    else
      return HBEM_MATRIX_RESIZING_FAILED ;
    newsize = write_cursor * sizeof( int64_t ) ;
    handle = realloc( Ai, newsize ) ;
    if ( handle )
    {
      Ai = (int64_t*) handle ;
      return 0 ;
    }
    return HBEM_MATRIX_RESIZING_FAILED ;
  }
  return 0 ;
}



int Matrix::Solve( Vector &x, const Vector &f )
{
  if ( f.size != cols )  return HBEM_MATRIX_VECTOR_MISMATCH ;
  if ( cols > 0 )
  {
    x.Resize( cols ) ;
    double *E = NULL ;
    void *Tree, *LU ;
    umfpack_dl_symbolic( cols, cols, Ap, Ai, Ax, &Tree, E, E ) ;
    umfpack_dl_numeric( Ap, Ai, Ax, Tree, &LU, E, E ) ;
    umfpack_dl_free_symbolic( &Tree ) ;
    umfpack_dl_solve( UMFPACK_A, Ap, Ai, Ax, x.f, f.f, LU, E, E ) ;
    umfpack_dl_free_numeric( &LU ) ;
    return 0 ;
  }
  return HBEM_UNDEFINED_MATRIX ;
}
