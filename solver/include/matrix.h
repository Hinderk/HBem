
#ifndef __HBEM_MATRIX_H
#define __HBEM_MATRIX_H

#include <cstdint>



namespace HBEM {

  class Vector ;


  class Matrix {

    public:

      Matrix( uint64_t size ) ;
      Matrix( void ) ;
     ~Matrix( void ) ;

      int Compress( double threshold ) ;
      int Solve( Vector &x, const Vector &f ) ;
      void Resize( uint64_t size ) ;

      double& operator()( uint64_t row, uint64_t col ) ;

    private:

      void Create( uint64_t size ) ;

    private:

      double   *Ax ;

      int64_t  *Ai ;
      int64_t  *Ap ;

      uint64_t  cols ;

  } ;

}

#endif // __HBEM_MATRIX_H
