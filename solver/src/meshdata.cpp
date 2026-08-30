
#include <algorithm>
#include <fstream>
#include <math.h>
#include <float.h>

#include "defines.h"
#include "vector.h"
#include "edgedata.h"
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
  Edge() ,
  Domains( 0 ) ,
  DomainId( NULL ) {}



void MeshData::Clear( void )
{
  Panel.clear() ;
  Edge.clear() ;
  xa = ya = DBL_MAX ;
  xo = yo = - DBL_MAX ;
  if ( Domains > 0 )  delete[] DomainId ;
  Domains = 0 ;
  DomainId = NULL ;
}



void MeshData::AddPanel( const PointData &NewPanel )
{
  Panel.push_back( NewPanel ) ;
  xa = NewPanel.midpoint.x < xa ? NewPanel.midpoint.x : xa ;
  xo = NewPanel.midpoint.x > xo ? NewPanel.midpoint.x : xo ;
  ya = NewPanel.midpoint.y < ya ? NewPanel.midpoint.y : ya ;
  yo = NewPanel.midpoint.y > yo ? NewPanel.midpoint.y : yo ;
}



void MeshData::AddEdge( const EdgeData &NewEdge )
{
  Edge.push_back( NewEdge ) ;
}



bool MeshData::MatchPanel( void )
{
  sort( Panel.begin(), Panel.end(), SegmentsOrdered ) ;
  bool BCok = true ;
  PointData *p0 = NULL ;
  for ( auto &p : Panel )
  {
    PointData *pold = p0 ;
    p0 = &p ;
    if ( pold )
    {
      if ( SegmentsOrdered( *pold, p ) )  continue ;
      p.interface = true ;
      p.match = pold -> index ;
      pold -> interface = true ;
      pold -> match = p.index ;
      p.epsilon_m = pold -> epsilon ;
      pold -> epsilon_m = p.epsilon ;
    }
    BCok &= p.interface || p.valid_bc ;
  }
  sort( Panel.begin(), Panel.end() ) ;
  return BCok ;
}


/*
void MeshData::InitDomains( const std::vector< int > &Mapping )
{
  if ( Domains > 0 )  delete[] DomainId ;
  Domains = Mapping.size() ;
  DomainId = NULL ;
  if ( Domains > 0 )
  {
    DomainId = new uint32_t[ Domains ] ;
    auto Hold( Mapping ) ;
    sort( Hold.begin(), Hold.end() ) ;
    for ( int n = 0 ; n < Domains ; n++ )  DomainId[ n ] = Hold[ n ] ;
  }
}  */



int MeshData::Assemble( Matrix &A, Vector &f ) const
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



int MeshData::Query( const Point2D &p0, Position *loc ) const
{
  int CurrentDomain = 0 ;
  int Winding = 0 ;
  int i = 0 ;
  for ( const auto &e : Edge )
  {
    if ( CurrentDomain < e.domain )
    {
      if ( Winding && i < 1 )  return CurrentDomain ;
      Winding = 0 ;
      CurrentDomain = e.domain ;
    }
    if ( p0.y < e.start.y + FLT_EPSILON )
    {
      if ( e.end.y < p0.y + FLT_EPSILON )
      {
        const int pos = e.Left( p0 ) ;
        if ( pos < 0 )
          Winding -- ;
        else if ( pos == 0 )
        {
          const double dx = p0.x - e.start.x ;
          const double dy = p0.y - e.start.y ;
          loc[ i ] = e ;
          loc[ i ++ ].arclen += dy * e.normal.x - dx * e.normal.y ;
        }
      }
      else if ( p0.dist( e.start ) < FLT_EPSILON )  // TODO: Are there
      {                                             // other edge cases
        loc[ i ++ ] = e ;                           // to consider?
      }
    }
    else if ( p0.y < e.end.y + FLT_EPSILON )
    {
      const int pos = e.Left( p0 ) ;
      if ( pos > 0 )
        Winding ++ ;
      else if ( pos == 0 )
      {
        const double dx = p0.x - e.start.x ;
        const double dy = p0.y - e.start.y ;
        loc[ i ] = e ;
        loc[ i ++ ].arclen += dy * e.normal.x - dx * e.normal.y ;
      }
    }
  }
  return i ? - i : Winding ? CurrentDomain : 0 ;
}



int MeshData::ComputeAreaData( AreaData &A0, const Vector &x ) const
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
  if ( A0.domain )
  {
    delete[] A0.domain ;
    delete[] A0.potential ;
  }
  const uint64_t LayerSize = A0.rows * A0.columns ;
  A0.domain = new uint32_t[ LayerSize ] ;
  A0.potential = new double[ LayerSize ] ;
  Position Location[ 1024 ] ;   
  double xc = A0.xa ;
  for ( uint32_t k = 0 ; k < A0.columns ; k ++ )
  {
    double yc = A0.ya ;
    for ( uint32_t l = 0 ; l < A0.rows ; l ++ )
    {
      const Point2D p0 = { xc, yc } ;
      int i = Query( p0, Location ) ;
      if ( i > 0 )
      {
        A0.domain[ k * A0.rows + l ] = i ;
        double &a = A0.potential[ k * A0.rows + l ] ;
        a = 0.0 ;
        for ( const auto &Q : Panel )
        {
          if ( Q.domain == i )
          {
            a -= DL( p0, Q ) * x( Q.index ) ;
            a += SL( p0, Q ) * x( Q.index + 1 ) ;
          }
        }
      }
      else if ( i < 0 )
      {
        double &a = A0.potential[ k * A0.rows + l ] ;
        a = 0 ;
        int n = -i ;
        while ( n -- )
        {
          const double so = Location[ n ].arclen ;
          int32_t il = LeftIndex( Location[ n ] ) ;
          if ( il < 0 )
            il = 0 ;
          else if ( FirstOffEdge( il ) )
            il ++ ;
          else if ( LastOnEdge( il ) )
            il -- ;
          const int32_t ir = il + 1 ;
          const double sl = Panel[ il ].arclen ;
          const double sr = Panel[ ir ].arclen ;
          const double xl = x( Panel[ il ].index ) ;
          const double xr = x( Panel[ ir ].index ) ;
          a += ( ( so - sl ) * xr + ( sr - so ) * xl ) / ( sr - sl ) ;
        }
        a /= - i ;
      }
      else
      {
        A0.domain[ k * A0.rows + l ] = 0 ;
        A0.potential[ k * A0.rows + l ] = NAN ;
      }
      yc += A0.dy ;
    }
    xc += A0.dx ;
  }
  return 0 ;
}



int32_t MeshData::LeftIndex( const Position &pos ) const
{
  if ( Panel.empty() || pos < Panel[ 0 ] )  return -1 ;
  int32_t ka = 0 ;
  int32_t ko = Panel.size() - 1 ;
  while ( ka < ko )
  {
    const int32_t kn = ( 1 + ka + ko ) >> 1 ;
    if ( pos < Panel[ kn ] )
      ko = kn - 1 ;
    else
      ka = kn ;
  }
  return ka ;
}



bool MeshData::FirstOffEdge( int32_t index ) const
{
  if ( Panel.size() < index + 2 )  return false ;
  const PointData &P0 = Panel[ index ] ;
  const PointData &PN = Panel[ index + 1 ] ;
  return P0.domain < PN.domain ? true : P0.boundary < PN.boundary ;
}



bool MeshData::LastOnEdge( int32_t index ) const
{
  if ( Panel.size() < index + 2 )  return true ;
  const PointData &P0 = Panel[ index ] ;
  const PointData &PN = Panel[ index + 1 ] ;
  return P0.domain < PN.domain ? true : P0.boundary < PN.boundary ;
}
