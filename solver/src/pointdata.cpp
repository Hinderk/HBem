
#include "pointdata.h"
#include <float.h>



bool SegmentsOrdered( const PointData &p, const PointData &q )
{
  return p.segment < q.segment                     ? true  :
         p.segment > q.segment                     ? false :
         p.midpoint.x + FLT_EPSILON < q.midpoint.x ? true  :
         p.midpoint.x - FLT_EPSILON > q.midpoint.x ? false :
         p.midpoint.y + FLT_EPSILON < q.midpoint.y ;
}



/*
bool PanelsOrdered( const PointData &p, const PointData &q )
{
  return p.domain < q.domain     ? true  :
         p.domain > q.domain     ? false :
         p.boundary < q.boundary ? true :
         p.boundary > q.boundary ? false :
         p.position < q.position ;
}
*/
