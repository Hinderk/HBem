
#include "areadata.h"
#include "defines.h"

#include <time.h>
#include <iomanip>
#include <fstream>




AreaData::AreaData( void ) :

  name() ,
  DomainId( NULL ) ,
  layers( 0 ) ,
  rows( 0 ) ,
  columns( 0 ) ,
  xa( 0.0 ) ,
  xo( 0.0 ) ,
  ya( 0.0 ) ,
  yo( 0.0 ) ,
  dx( 0.0 ) ,
  dy( 0.0 ) ,
  potential( NULL ) ,
  domain( NULL ) {}



AreaData::AreaData( double MeshWidth, double MeshHeight ) :

  name() ,
  DomainId( NULL ) ,
  layers( 0 ) ,
  rows( 0 ) ,
  columns( 0 ) ,
  xa( 0.0 ) ,
  xo( 0.0 ) ,
  ya( 0.0 ) ,
  yo( 0.0 ) ,
  dx( MeshWidth ) ,
  dy( MeshHeight ) ,
  potential( NULL ) ,
  domain( NULL ) {}



AreaData::~AreaData( void )
{
  if ( domain )
  {
    delete[] domain ;
    delete[] potential ;
    delete[] DomainId ;
  }
}



void AreaData::SetCorner( double xval, double yval )
{
  xa = xval < xa ? xval : xa ;
  xo = xval > xo ? xval : xo ;
  ya = yval < ya ? yval : ya ;
  yo = yval > yo ? yval : yo ;
}



int AreaData::Write( const char *File, PlotMode Mode ) const
{
  switch( Mode )
  {
    case VISIT:
      return WriteVisit( File ) ;
    case GNUPLOT:
      return WriteGnuplot( File ) ;
    default: ;
  }
  return HBEM_UNKNOWN_OUTPUT_FORMAT ;
}



int AreaData::WriteGnuplot( const char *File ) const
{
  time_t utc = time( NULL ) ;
  struct tm *gm = gmtime( &utc ) ;
  char Time[ 128 ] ;
  strftime( Time, 128, "%x -- %T", gm ) ;
  FILE *Out = fopen( File, "w" ) ;
  if ( Out )
  {
    fprintf( Out, "#.type:\n# hbem-area-data\n#.format:\n" ) ;
    fprintf( Out, "# %s\n#.name:", HBEM_AREA_FILE_FORMAT ) ;
    fprintf( Out, "\n# %s\n#", name.c_str() ) ;
    uint64_t Layer = rows * columns ;
    for ( uint32_t l = 0 ; l < layers ; l ++ )
    {
      fprintf( Out, "\n# Layer: %i", DomainId[ l ] ) ;
      double xval = xa ;
      for ( uint32_t n = 0 ; n < columns ; n ++ )
      {
        double yval = ya ;
        for ( uint32_t m = 0 ; m < rows ; m ++ )
        {
          double zval = potential[ l * Layer + n * rows + m ] ;
          fprintf( Out, "\n%+12.8f  %+12.8f ", xval, yval ) ;
          fprintf( Out, "  %+16.12e", zval ) ;
          yval += dy ;
        }
        xval += dx ;
        fprintf( Out, "\n" ) ;
      }
      fprintf( Out, "\n" ) ;
    }
    fprintf( Out, "#.end\n" ) ;
    fclose( Out ) ;
    return 0 ;
  }
  return HBEM_FAILED_TO_WRITE_FILE ;
}



int AreaData::WriteVisit( const char *File ) const
{
  std::stringstream FileName ;
  uint64_t Layer = rows * columns ;
  for ( uint32_t l = 0 ; l < layers ; l ++ )
  {
    FileName.clear() ;
    FileName << File ;
    FileName << "-" << std::setw(4) << std::setfill('0') ;
    FileName << DomainId[ l ] << ".dat" ;
    FILE *Out = fopen( FileName.str().c_str(), "w" ) ;
    if ( Out )
    {
      fprintf( Out, "x y potential\n" ) ;
      double xval = xa ;
      for ( uint32_t n = 0 ; n < columns ; n ++ )
      {
        double yval = ya ;
        for ( uint32_t m = 0 ; m < rows ; m ++ )
        {
          double zval = potential[ l * Layer + n * rows + m ] ;
          fprintf( Out, "%f %f %f\n", xval, yval, zval ) ;
          yval += dy ;
        }
        xval += dx ;
      }
      fclose( Out ) ;
    }
  }
  return HBEM_FAILED_TO_WRITE_FILE ;
}
