
#ifndef __HBEM_CONTAINER 
#define __HBEM_CONTAINER 

#include <cstdint>

class BEM_Function ;



namespace HBEM {


  struct Point2D {

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


  struct Container {

    bool          StartNewBoundary ;
    bool          StartNewSegment ;

    int           State ;

    double        MeshWidth ;
    double        MinimalWidth ;
    double        MaximalWidth ;

    BEM_Function *f0 ;
    RobinBC       BoundaryCondition ; 

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
