
#ifndef __HBEM_POINTDATA_H
#define __HBEM_POINTDATA_H

#include <cstdint>



struct PointData {

  uint64_t index ;
  int64_t  match ;

  int32_t  segment ;

  double   x ;
  double   y ;
  double   normal_x ;
  double   normal_y ;

} ;

#endif     // __HBEM_POINTDATA_H
