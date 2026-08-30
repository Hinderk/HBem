
#ifndef __HBEM_POINTDATA_H
#define __HBEM_POINTDATA_H

#include "container.h"




struct PointData : public HBEM::Position {

  bool           interface ;
  bool           valid_bc ;

  uint64_t       index ;
  uint64_t       match ;

  int32_t        segment ;

  int32_t        material ;

  double         epsilon ;
  double         epsilon_m ;

  double         panelsize ;

  HBEM::Point2D  midpoint ;
  HBEM::Point2D  normal ;

  double         c0, c1, cr ;

} ;


bool SegmentsOrdered( const PointData &p, const PointData &q ) ;


#endif     // __HBEM_POINTDATA_H
