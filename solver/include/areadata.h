
#ifndef __HBEM_AREADATA_H
#define __HBEM_AREADATA_H

#include <cstdint>
#include <string>

class MeshData ;


enum PlotMode {

  VISIT = 1 ,
  GNUPLOT = 2

} ;



class AreaData {

  friend class MeshData ;

  public:

    AreaData( double MeshWidth, double MeshHeight ) ;
    AreaData( void ) ;
   ~AreaData( void ) ;

    void Resize( double MeshWidth, double MeshHeight ) ;
    void ClearCorners( void ) ;
    void SetCorner( double xval, double yval ) ;
    int Write( const char *File , PlotMode = VISIT ) const ;
    int WriteVisit( const char *File ) const ;
    int WriteGnuplot( const char *File ) const ;

  private:

    std::string  name ;

    uint32_t    *DomainId ;
    uint32_t     layers ;
    uint32_t     rows ;
    uint32_t     columns ;

    double       xa, xo ;
    double       ya, yo ;

    double       dx, dy ;

    double      *potential ;

    uint32_t    *domain ;

} ;

#endif // __HBEM_AREADATA_H
