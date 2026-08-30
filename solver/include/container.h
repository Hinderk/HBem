
#ifndef __HBEM_CONTAINER
#define __HBEM_CONTAINER

#include <cstdint>
#include <cmath>
#include <map>
#include <list>
#include <string>

class BEM_Function ;



namespace HBEM {


  struct Point2D {

    double dist( const Point2D &q ) const
    {
      return fabs( x - q.x ) + fabs( y - q.y ) ;
    }

    double x ;
    double y ;

  } ;


  struct Line2D {

    int32_t               Material ;
    std::list< int32_t >  Vertices ;

  } ;


  struct RobinBC {

    double c0 ;            // Function value
    double c1 ;            // Normal derivative
    double cr ;            // RHS of boundary c.

  } ;


  struct Boundary2D {

    std::string                   f ;
    std::list< int32_t >          Segments ;
    std::map< int32_t, RobinBC >  BC ;

  } ;


  struct Domain2D {

    double                           epsilon ;
    std::map< int32_t, Boundary2D >  Boundaries ;

  } ;


  struct Position {

    bool operator<( const Position &pos ) const
    {
      return domain < pos.domain       ? true  :
             domain > pos.domain       ? false :
             boundary < pos.boundary   ? true  :
             boundary > pos.boundary   ? false :
             arclen < pos.arclen ;
    }

    int32_t  domain ;
    int32_t  boundary ;

    double   arclen ;

  } ;


  struct Container {

    bool          StartNewBoundary ;
    bool          StartNewSegment ;

    int           State ;

    double        MeshWidth ;
    double        MinimalWidth ;
    double        MaximalWidth ;

    double        ArcLength ;

    BEM_Function *f0 ;
    RobinBC       BoundaryCondition ;

    double        epsilon ;

    uint64_t      CurrentDOFIndex ;
    uint64_t      DOFIndexCounter ;

    int32_t       FirstVertex ;
    int32_t       LastVertex ;
    int32_t       CurrentVertex ;
    int32_t       LastSegment ;
    int32_t       CurrentSegment ;
    int32_t       CurrentMaterial ;
    int32_t       CurrentDomain ;
    int32_t       CurrentBoundary ;

  } ;

}

#endif  // __HBEM_CONTAINER
