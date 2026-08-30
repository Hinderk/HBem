
#ifndef __HBEM_EDGEDATA_H
#define __HBEM_EDGEDATA_H

#include "container.h"
#include <float.h>



struct EdgeData : public HBEM::Position {

  int Left( const HBEM::Point2D &p0 ) const
  {
    const double dx = p0.x - start.x ;
    const double dy = p0.y - start.y ;
    const double dn = dx * normal.x + dy * normal.y ;
    return dn < - FLT_EPSILON ? 1 : dn > FLT_EPSILON ? -1 : 0 ;
  }

  HBEM::Point2D  start ;
  HBEM::Point2D  end ;

  HBEM::Point2D  normal ;

} ;

#endif     // __HBEM_EDGEDATA_H
