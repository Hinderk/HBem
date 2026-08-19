
#ifndef __HBEM_POINTDATA_H
#define __HBEM_POINTDATA_H

#include <cstdint>
#include <float.h>


struct PointData {


  bool operator<( const PointData &p )
  {
    return segment < p.segment   ? true  :
	   segment > p.segment   ? false :
           x + FLT_EPSILON < p.x ? true  :
           x - FLT_EPSILON > p.x ? false :
           y + FLT_EPSILON < p.y ;
  }

  bool     interface ;
  bool     valid_bc ;

  uint64_t index ;
  uint64_t match ;

  int32_t  segment ;

  int32_t  domain ;
  int32_t  boundary ;
  int32_t  material ;

  double   panelsize ;
  double   x ;
  double   y ;
  double   normal_x ;
  double   normal_y ;

  double   c0, c1, cr ;

} ;

#endif     // __HBEM_POINTDATA_H
