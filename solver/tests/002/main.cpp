
#include <iostream>
#include <cstring>
#include <math.h>

#include "container.h"
#include "defines.h"
#include "pointdata.h"

using namespace HBEM ;




static double DL( const Point2D q, const PointData &p )
{
  constexpr double D0 = 0.5 / M_PI ;
  const double dx = q.x - p.midpoint.x ;
  const double dy = q.y - p.midpoint.y ;
  const double delta = dx * p.normal.x + dy * p.normal.y ;
  const double s0 = - dx * p.normal.y + dy * p.normal.x ;
  const double h = 0.5 * p.panelsize ;
  if ( fabs(s0) < h )
  {
    const double a1 = atan2( delta, s0 + h ) ;
    const double a2 = atan2( delta, h - s0 ) ;
    return ( delta < 0 ? -.5 : .5 ) - D0 * ( a1 + a2 ) ;
  }
  const double a1 = atan2( delta, s0 + h ) ;
  const double a2 = atan2( delta, s0 - h ) ;
  return D0 * ( a2 - a1 ) ;
}



static double DL0( const Point2D q, const PointData &p )
{
  constexpr double D0 = 0.5 / M_PI ;
  const double dx = q.x - p.midpoint.x ;
  const double dy = q.y - p.midpoint.y ;
  const double delta = dx * p.normal.x + dy * p.normal.y ;
  const double r2 =  dx * dx + dy * dy ;
  return D0 * p.panelsize * delta / r2 ;
}



static double SL( const Point2D q, const PointData &p )
{
  constexpr double S0 = 0.5 / M_PI ;
  const double dx = q.x - p.midpoint.x ;
  const double dy = q.y - p.midpoint.y ;
  const double delta = dx * p.normal.x + dy * p.normal.y ;
  const double s0 = - dx * p.normal.y + dy * p.normal.x ;
  const double h = 0.5 * p.panelsize ;
  const double a1 = atan2( delta, s0 + h ) ;
  const double a2 = atan2( delta, s0 - h ) ;
  const double d1 = delta * delta + ( s0 + h ) * ( s0 + h ) ;
  const double d2 = delta * delta + ( s0 - h ) * ( s0 - h ) ;
  const double l1 = ( 1.0 - 0.5 * log( d1 ) ) * ( s0 + h ) ;
  const double l2 = ( 1.0 - 0.5 * log( d2 ) ) * ( s0 - h ) ;
  return S0 * ( delta * ( a1 - a2 ) + l1 - l2 ) ;
}



static double SL0( const Point2D q, const PointData &p )
{
  constexpr double S0 = -0.25 / M_PI ;
  const double d2 = ( q.x - p.midpoint.x ) * ( q.x - p.midpoint.x )
                  + ( q.y - p.midpoint.y ) * ( q.y - p.midpoint.y ) ;
  return S0 * log( d2 ) * p.panelsize ;
}



static double SL00( double PanelSize )
{
  constexpr double S0 = 0.5 / M_PI ;
  constexpr double L0 = 1.0 - log( 0.5 ) ;
  return S0 * ( L0 - log( PanelSize ) ) * PanelSize ;
}



int main( int argc, const char *argv[] )
{
  PointData PD ;
  PD.midpoint = { 2.0, -4.0 } ;
  PD.normal = { -1, 0 } ;
  PD.panelsize = 0.2 ;
  Point2D P[] = { { 2.1, -3.9 }, { 2.1, -6 }, { 2.1, -2 }, { 2.1, -4.1 },
                  { 2.0, -4 }, { 2.2, -4 }, { 5, -4 }, { 1.5, -4 },
                  { 1.999, -4 }, { 2.0001, -4 } } ;
  for ( const auto &Q : P )
  {
    std::cout << "# DL Point data:  {" << Q.x << "," << Q.y << "}\n" ;
    std::cout << "# DL Result: " << DL( Q, PD ) << "\n" ;
    std::cout << "# DL Approximation: " << DL0( Q, PD ) << "\n\n" ;
    std::cout << "# SL Point data:  {" << Q.x << "," << Q.y << "}\n" ;
    std::cout << "# SL Result: " << SL( Q, PD ) << "\n" ;
    std::cout << "# SL(0) Approximation: " << SL0( Q, PD ) ;
    std::cout << "  --  " << SL00( PD.panelsize ) << "\n\n" ;
  }
  return 0 ;
}
