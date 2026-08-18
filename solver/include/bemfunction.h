
#ifndef __HBEM_BEMFUNCTION_H
#define __HBEM_BEMFUNCTION_H

#include <cstdint>
#include <stdlib.h>

#include "pointdata.h"





struct BEM_Function {

    BEM_Function( void ) {}
    virtual ~BEM_Function( void ) {}

    virtual int Evaluate( PointData &MeshPoint ) = 0 ;
    virtual const char* QueryType( void ) const = 0 ;

} ;

#endif // __HBEM_BEMFUNCTION_H
