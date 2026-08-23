
#ifndef __HMB_BCC_H
#define __HMB_BCC_H

#include "bemfunction.h"
#include "defines.h"
#include "export_BCC.h"



extern "C" {

  BCC_EXPORT int32_t HBEM_Init( const Service_t * ) ;
  BCC_EXPORT int32_t HBEM_Exit( void ) ;

} ;




class BC_C : public BEM_Function {

  public:

    BC_C( void ) {}
   ~BC_C( void ) {}

    int Evaluate( PointData &MeshPoint ) ;

    const char* QueryType( void ) const { return Type ; }
    PluginVersion_t Revision( void ) const { return Version ; }

    static constexpr PluginVersion_t Version = { 0, 1 } ;
    static constexpr const char *Type = "BC_C" ;

} ;


#endif     // __HMB_BCC_H
