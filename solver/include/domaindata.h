
#ifndef __HBEM_DOMAINDATA_H
#define __HBEM_DOMAINDATA_H

#include <stdio.h>
#include <list>
#include <map>
#include <string>

#include "defines.h"
#include "pointdata.h"



class DomainData {

  struct Point2D {

    double x ;
    double y ;

  } ;

  struct Line2D {

    int             Material ;
    std::list<int>  Vertices ;

  } ;

  struct RobinBC {

    double c0 ;            // Function value
    double c1 ;            // Normal derivative
    double cr ;            // RHS of boundary c.

  } ;

  struct Boundary2D {

    std::string            f ;
    std::list<int>         Segments ;
    std::map<int,RobinBC>  BC ;

  } ;

  struct Domain2D {

    double                    epsilon ;
    std::map<int,Boundary2D>  Boundaries ;

  } ;

  public:

    DomainData( int Level = HBEM_DEFAULT_VERBOSITY ) ;

    int Write( const char *File ) const ;
    int Read( const char *File, const char *Path = NULL ) ;
    int SetLog( FILE *LogFile ) ;
    int Start( void ) ;
    int Stop( void ) ;
    int Search( const char *Path = NULL ) ;

  private:

    void Clear( void ) ;

    int LogLevel ;
    FILE *Log ;

    std::string               SearchPath ;
    std::string               Name ;

    std::map<int,Point2D>     Points ;
    std::map<int,Line2D>      Segments ;

    std::map<int,Domain2D>    Patches ;

} ;


#endif    // __HBEM_DOMAINDATA_H
