
#ifndef __HMB_BCB_H
#define __HMB_BCB_H

#include "bemfunction.h"
#include "defines.h"
#include "export_BCB.h"



extern "C" {

  BCB_EXPORT int32_t HBEM_Init( const Service_t * ) ;
  BCB_EXPORT int32_t HBEM_Exit( void ) ;

} ;




class BC_B : public BEM_Function {

  public:

    BC_B( void ) {}
   ~BC_B( void ) {}

    int Evaluate( PointData &MeshPoint ) ;

    const char* QueryType( void ) const { return Type ; }
    PluginVersion_t Revision( void ) const { return Version ; }

    static constexpr PluginVersion_t Version = { 0, 1 } ;
    static constexpr const char *Type = "BC_B" ;

} ;


#endif     // __HMB_BCB_H
