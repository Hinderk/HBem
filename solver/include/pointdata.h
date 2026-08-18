
#ifndef __HBEM_POINTDATA_H
#define __HBEM_POINTDATA_H

#include <float.h>


struct PointData {


  bool operator<( const PointData &p )
  {
    if ( domain == p.domain )
    {
      if ( x + FLT_EPSILON < p.x )  return true ;
      if ( x - FLT_EPSILON > p.x )  return false ;
      return( y + FLT_EPSILON < p.y ) ;
    }
    return( domain < p.domain ) ;
  }


  int      index ;
  int      match ;

  int      domain ;
  int      boundary ;
  int      material ;

  double   panelsize ;
  double   x ;
  double   y ;
  double   normal_x ;
  double   normal_y ;

} ;

#endif     // __HBEM_POINTDATA_H
