
#include <algorithm>
#include <fstream>
#include <math.h>

#include "defines.h"
#include "vector.h"
#include "areadata.h"
#include "meshdata.h"

using namespace HBEM ;




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




extern "C" int Compare_uint32_t( const void *a, const void *b )
{
  const uint32_t aval = *( (uint32_t*) a ) ;
  const uint32_t bval = *( (uint32_t*) b ) ;
  return aval < bval ? -1 : bval < aval ? 1 : 0 ;
}



MeshData::MeshData() :

  xa( + DBL_MAX ) ,
  xo( - DBL_MAX ) ,
  ya( + DBL_MAX ) ,
  yo( - DBL_MAX ) ,
  Panel() ,
  DomainStart() ,
  DomainEnd() {}



void MeshData::Clear( void )
{
  DomainStart.clear() ;
  DomainEnd.clear() ;
  Panel.clear() ;
  xa = ya = DBL_MAX ;
  xo = yo = - DBL_MAX ;
}



void MeshData::Start( uint32_t Domain, uint64_t DOFIndex )
{
  DomainStart[ Domain ] = DOFIndex ;
}



void MeshData::Stop( uint32_t Domain, uint64_t DOFIndex )
{
  DomainEnd[ Domain ] = DOFIndex ;
}



void MeshData::AddPanel( const PointData &NewPanel )
{
  Panel.push_back( NewPanel ) ;
  xa = NewPanel.midpoint.x < xa ? NewPanel.midpoint.x : xa ;
  xo = NewPanel.midpoint.x > xo ? NewPanel.midpoint.x : xo ;
  ya = NewPanel.midpoint.y < ya ? NewPanel.midpoint.y : ya ;
  yo = NewPanel.midpoint.y > yo ? NewPanel.midpoint.y : yo ;
}



bool MeshData::MatchPanel( void )
{
  sort( Panel.begin(), Panel.end() ) ;
  bool BCok = true ;
  PointData *p0 = NULL ;
  for ( auto &p : Panel )
  {
    PointData *pold = p0 ;
    p0 = &p ;
    if ( pold )
    {
      if ( *pold < p )  continue ;
      p.interface = true ;
      p.match = pold -> index ;
      pold -> interface = true ;
      pold -> match = p.index ;
      p.epsilon_m = pold -> epsilon ;
      pold -> epsilon_m = p.epsilon ;
    }
    BCok &= p.interface || p.valid_bc ;
  }
  return BCok ;
}



int MeshData::Assemble( HBEM::Matrix &A, HBEM::Vector &f )
{
  const uint64_t ntotal = 2 * Panel.size() ;
  A.Resize( ntotal ) ;
  f.Resize( ntotal ) ;
  uint64_t nc = 0 ;
  for ( const auto &P : Panel )
  {
    for ( const auto &Q : Panel )
    {
      if ( P.domain == Q.domain )
      {
        A( nc, Q.index ) = DL( P.midpoint, Q ) ;
        A( nc, Q.index + 1 ) = - SL( P.midpoint, Q ) ;
      }
    }
    if ( P.interface )
    {
      nc ++ ;
      A( nc, P.index ) = 1.0 ;
      A( nc, P.match ) = -1.0 ;
      nc ++ ;
      A( nc, P.index + 1 ) = P.epsilon ;
      A( nc, P.match + 1 ) = P.epsilon_m ;
    }
    else
    {
      nc ++ ;
      A( nc, P.index ) = P.c0 ;
      A( nc, P.index + 1 ) = P.c1 ;
      f( nc ) = P.cr ;
    }
    nc ++ ;
  }
  int state = A.Compress( FLT_EPSILON ) ;
  return nc == ntotal ? state : HBEM_COLLOCATION_MATRIX_BROKEN ;
}



int MeshData::ComputeAreaData( AreaData &A0, const HBEM::Vector &x )
{
  A0.name = Name ;
  if ( A0.dx < FLT_EPSILON || A0.dy < FLT_EPSILON )
    return HBEM_INSUFFICIENT_AREADATA ;
  A0.SetCorner( xa, ya ) ;
  A0.SetCorner( xo, yo ) ;
  A0.columns = 2 + ( A0.xo - A0.xa ) / A0.dx ;
  A0.rows = 2 + ( A0.yo - A0.ya ) / A0.dy ;
  A0.dx = ( A0.xo - A0.xa ) / ( A0.columns - 1 ) ;
  A0.dy = ( A0.yo - A0.ya ) / ( A0.rows - 1 ) ;
  A0.layers = DomainEnd.size() ;
  if ( A0.domain )
  {
    delete[] A0.domain ;
    delete[] A0.potential ;
    delete[] A0.DomainId ;
  }
  const uint64_t LayerSize = A0.rows * A0.columns ;
  A0.DomainId = new uint32_t[ A0.layers ] ;
  A0.domain = new uint32_t[ LayerSize ] ;
  A0.potential = new double[ A0.layers * LayerSize ] ;
  uint32_t n = 0 ;
  for ( const auto &I : DomainEnd )  A0.DomainId[ n ++ ] = I.first ;
  qsort( A0.DomainId, n, sizeof(uint32_t), Compare_uint32_t ) ;
  double xc = A0.xa ;
  for ( uint32_t k = 0 ; k < A0.columns ; k ++ )
  {
    double yc = A0.ya ;
    for ( uint32_t l = 0 ; l < A0.rows ; l ++ )
    {
      const Point2D p0 = { xc, yc } ;
      for ( const auto &Q : Panel )
      {
        for ( n = 0 ; A0.DomainId[n] < Q.domain ; n ++ ) ;
        double &a = A0.potential[ n * LayerSize + k * A0.rows + l ] ;
        a -= DL( p0, Q ) * x( Q.index ) ;
        a += SL( p0, Q ) * x( Q.index + 1 ) ;
      }
      yc += A0.dy ;
    }
    xc += A0.dx ;
  }
  return 0 ;
}
