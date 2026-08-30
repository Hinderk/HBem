
#ifndef __HBEM_MESHDATA_H
#define __HBEM_MESHDATA_H

#include <vector>

#include "pointdata.h"
#include "areadata.h"
#include "edgedata.h"
#include "matrix.h"
#include "vector.h"
#include "container.h"




class MeshData {

  public:

    MeshData( void ) ;
   ~MeshData( void ) {}

   void Clear( void ) ;
   void SetName( std::string &NewName )  { Name = NewName ; }
   bool MatchPanel( void ) ;
   void AddEdge( const EdgeData &NewEdge ) ;
   void AddPanel( const PointData &NewPanel ) ;
   int Assemble( HBEM::Matrix &A, HBEM::Vector &f ) const ;
   int ComputeAreaData( AreaData &A0, const HBEM::Vector &x ) const ;
   int Query( const HBEM::Point2D &p0, HBEM::Position *loc ) const ;
   int32_t LeftIndex( const HBEM::Position &loc ) const ;
   bool FirstOffEdge( int32_t index ) const ;
   bool LastOnEdge( int32_t index ) const ;

  private:

    std::string                     Name ;

    double                          xa, xo ;
    double                          ya, yo ;

    std::vector< PointData >        Panel ;
    std::vector< EdgeData >         Edge ;

    uint32_t                        Domains ;
    uint32_t                       *DomainId ;

} ;

#endif // __HBEM_MESHDATA_H
