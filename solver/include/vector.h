
#ifndef __HBEM_VECTOR_H
#define __HBEM_VECTOR_H

#include <cstdint>



namespace HBEM {

  class Matrix ;


  class Vector {

    friend class Matrix ;

    public:

      Vector( uint64_t length ) ;
      Vector( void ) ;
     ~Vector( void ) ;

      void Resize( uint64_t length ) ;

      double  operator()( uint64_t index ) const ;
      double& operator()( uint64_t index ) ;

    private:

      double    *f ;
      uint64_t   size ;

  } ;

}

#endif // __HBEM_VECTOR_H
