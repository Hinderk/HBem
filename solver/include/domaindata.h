
#ifndef __HBEM_DOMAINDATA_H
#define __HBEM_DOMAINDATA_H

#include <stdio.h>
#include <memory>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include "container.h"
#include "bemplugin.h"
#include "bemfunction.h"

class MeshData ;



class DomainData {

  public:

    DomainData( void ) ;
   ~DomainData( void ) ;

    int Load( const char *Dir ) ;
    int Write( const char *File ) const ;
    int Read( const char *File, const char *Path = NULL ) ;
    int SetLog( FILE *LogFile ) ;
    int ChangeLogLevel( int NewLevel ) ;
    int Start( void ) ;
    int Stop( void ) ;
    int Search( const char *Path = NULL ) ;
    int LogInfo( std::stringstream &msg, int Nesting = 0 ) const ;
    int LogInfo( const char *Message, int Nesting = 0 ) const ;
    int LogAlert( std::stringstream &msg, int Nesting = 0 ) const ;
    int LogAlert( const char *Message, int Nesting = 0 ) const ;
    int Update( const char *Name, const Plugin_t *Data ) ;
    int CreateMesh( MeshData &Mesh, double MeshWidth ) ;

  private:

    int Create( BEM_Function *&f, const char *Type ) ;
    int Discard( const BEM_Function *f ) ;
    int Unload( void ) ;
    int Load( const char *Name, const Service_t *Service ) ;
    int TraceBoundary( MeshData &Mesh, HBEM::Container &VertexData ) ;

  private:

    int LogLevel ;
    FILE *Log ;

    std::string                               ModulePath ;
    std::string                               Name ;

    std::map< int, HBEM::Point2D >            Points ;
    std::map< int, HBEM::Line2D >             Segments ;

    std::map< int, HBEM::Domain2D >           Patches ;

    std::list< std::shared_ptr<BEM_Plugin> >  Module ;
    std::map< std::string, Plugin_t >         Registry ;
    Service_t                                 Service ;
    PluginVersion_t                           MaxVersion ;

} ;

#endif    // __HBEM_DOMAINDATA_H
