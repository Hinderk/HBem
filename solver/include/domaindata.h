
#ifndef __HBEM_DOMAINDATA_H
#define __HBEM_DOMAINDATA_H

#include <stdio.h>
#include <memory>
#include <list>
#include <map>
#include <string>

#include "bemplugin.h"
#include "bemfunction.h"

class MeshData ;



class DomainData {


  friend class MeshData ;


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

    DomainData( void ) ;
   ~DomainData( void ) ;

    int Load( const char *Dir ) ;
    int Write( const char *File ) const ;
    int Read( const char *File, const char *Path = NULL ) ;
    int SetLog( FILE *LogFile ) ;
    int Start( void ) ;
    int Stop( void ) ;
    int Search( const char *Path = NULL ) ;
    int LogInfo( const char *Message, int Nesting = 0 ) const ;
    int LogAlert( const char *Message, int Nesting = 0 ) const ;
    int Update( const char *Name, const Plugin_t *Data ) ;

    int ChangeLogLevel( int NewLevel ) ;

  public:

    int Create( BEM_Function *&f, const char *Type ) ;
    int Discard( const BEM_Function *f ) ;
    int Unload( void ) ;
    int Load( const char *Name, const Service_t *Service ) ;

  private:

    int LogLevel ;
    FILE *Log ;

    std::string                               ModulePath ;
    std::string                               Name ;

    std::map< int, Point2D >                  Points ;
    std::map< int, Line2D >                   Segments ;

    std::map< int, Domain2D >                 Patches ;

    std::list< std::shared_ptr<BEM_Plugin> >  Module ;
    std::map< std::string, Plugin_t >         Registry ;
    Service_t                                 Service ;
    PluginVersion_t                           MaxVersion ;

} ;


#endif    // __HBEM_DOMAINDATA_H
