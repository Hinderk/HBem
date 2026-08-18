
#ifndef __HBEM_MESHDATA_H
#define __HBEM_MESHDATA_H

#include "pointdata.h"
#include <map>
#include <vector>

class DomainData ;


class MeshData {

  public:

    MeshData( void ) ;
   ~MeshData( void ) {}

    int CreateMesh( const DomainData &D, double h ) ;

  private:

    std::vector<PointData>  Panel ;

    std::map<int,double>    epsilon ;

    std::map<int,int>       DomainStart ;
    std::map<int,int>       DomainEnd ;

} ;

#endif // __HBEM_MESHDATA_H
