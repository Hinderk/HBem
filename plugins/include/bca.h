
#ifndef __HMB_BCA_H
#define __HMB_BCA_H

#include "bemfunction.h"
#include "defines.h"
#include "export_BCA.h"





extern "C" {

  BCA_EXPORT int32_t HBEM_Init( const Service_t * ) ;
  BCA_EXPORT int32_t HBEM_Exit( void ) ;

} ;




class BC_A : public BEM_Function {

  public:

    BC_A( void ) {}
   ~BC_A( void ) {}

    int Evaluate( PointData &MeshPoint ) ;

    const char* QueryType( void ) const { return Type ; } 
    PluginVersion_t Revision( void ) const { return Version ; }

    static constexpr PluginVersion_t Version = { 0, 1 } ;
    static constexpr const char *Type = "BC_A" ;

} ;


#endif     // __HMB_BCA_H
