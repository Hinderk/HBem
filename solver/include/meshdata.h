
#ifndef __HBEM_MESHDATA_H
#define __HBEM_MESHDATA_H

#include <map>
#include <vector>

#include "pointdata.h"
#include "areadata.h"
#include "matrix.h"
#include "vector.h"



class MeshData {

  public:

    MeshData( void ) ;
   ~MeshData( void ) {}

   void Start( uint32_t Domain, uint64_t DOFIndex ) ;
   void Stop( uint32_t Domain, uint64_t DOFIndex ) ;
   void Clear( void ) ;
   void SetName( std::string &NewName )  { Name = NewName ; }
   bool MatchPanel( void ) ;
   void AddPanel( const PointData &NewPanel ) ;
   int Assemble( HBEM::Matrix &A, HBEM::Vector &f ) ;
   int ComputeAreaData( AreaData &A0, const HBEM::Vector &x ) ;

  private:

    std::string                     Name ;

    double                          xa, xo ;
    double                          ya, yo ;

    std::vector< PointData >        Panel ;

    std::map< uint32_t, uint64_t >  DomainStart ;
    std::map< uint32_t, uint64_t >  DomainEnd ;

} ;

#endif // __HBEM_MESHDATA_H
