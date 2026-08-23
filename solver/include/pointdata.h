
#ifndef __HBEM_POINTDATA_H
#define __HBEM_POINTDATA_H

#include "container.h"
#include <float.h>


struct PointData {


  bool operator<( const PointData &p )
  {
    return segment < p.segment   ? true  :
           segment > p.segment   ? false :
           midpoint.x + FLT_EPSILON < p.midpoint.x ? true  :
           midpoint.x - FLT_EPSILON > p.midpoint.x ? false :
           midpoint.y + FLT_EPSILON < p.midpoint.y ;
  }

  bool           interface ;
  bool           valid_bc ;

  uint64_t       index ;
  uint64_t       match ;

  int32_t        segment ;

  int32_t        domain ;
  int32_t        boundary ;
  int32_t        material ;

  double         epsilon ;
  double         epsilon_m ;

  double         panelsize ;

  HBEM::Point2D  midpoint ;
  HBEM::Point2D  normal ;

  double         c0, c1, cr ;

} ;

#endif     // __HBEM_POINTDATA_H
