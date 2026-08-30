
#include <fstream>
#include <sstream>
#include <math.h>
#include <float.h>

#include "defines.h"
#include "pointdata.h"
#include "meshdata.h"
#include "openfile.h"
#include "domaindata.h"

#ifdef _WIN32
  #include "windows/getline.h"
  #include "windows/dirent.h"
#endif


using namespace HBEM ;



static int Register( void *Handle, const char *Name, const Plugin_t *Data )
{
  return ( (DomainData*) Handle ) -> Update( Name, Data ) ;
}



DomainData::DomainData( void ) :

  Log( NULL ) ,
  LogLevel( HBEM_DEFAULT_VERBOSITY ) ,
  ModulePath () ,
  Name() ,
  Points() ,
  Segments() ,
  Patches() ,
  DomainId() ,
  Module() ,
  Registry()
{
  Service.Version = { 0, 0 } ;
  Service.Register = Register ;
  Service.Request = NULL ;
  Service.Registrar = this ;
  MaxVersion = { 65535, 65535 } ;
}



int DomainData::Load( const char *Dir )
{
  FILE *Out = Log ? Log : stdout ;
  char WorkingArray[ HBEM_MAX_FILE_PATH_LENGTH + 1 ] = { 0 } ;
  char NameArray[ 2 * HBEM_MAX_FILE_PATH_LENGTH + 1 ] = { 0 } ;
  int OldCount = Registry.size() ;
  if ( Dir )
    strncpy( WorkingArray, Dir, HBEM_MAX_FILE_PATH_LENGTH ) ;
  else
  {
    const char *Paths = getenv( HBEM_PLUGIN_DIRECTORY ) ;
    if ( Paths == NULL )  return HBEM_PLUGIN_DIRECTORY_NOT_FOUND ;
    strncpy( WorkingArray, Paths, HBEM_MAX_FILE_PATH_LENGTH ) ;
  }
  struct stat fs ;
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
    fprintf( Out, "\n#I -- Scanning file system for plugins ..." ) ;
  char *Directory = strtok( WorkingArray, ":" ) ;
  while ( Directory != NULL )
  {
    DIR *DirHandle = opendir( Directory ) ;
    if ( DirHandle != NULL )
    {
      if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
        fprintf( Out, "\n#I -- -- Scanning directory:  %s", Directory ) ;
      strncpy( NameArray, Directory, HBEM_MAX_FILE_PATH_LENGTH ) ;
      int Cursor = strlen( Directory ) ;
      NameArray[ Cursor ++ ] = '/' ;
      struct dirent *DirEntry = readdir( DirHandle ) ;
      for ( ; DirEntry ; DirEntry = readdir( DirHandle ) )
      {
        const char *Name = DirEntry -> d_name ;
        strncpy( NameArray + Cursor, Name, HBEM_MAX_FILE_PATH_LENGTH ) ;
        if ( stat( NameArray, &fs ) < 0 )  continue ;
        if ( S_ISREG( fs.st_mode ) )
        {
          if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
            fprintf( Out, "\n#I -- -- -- Loading module:  %s", Name ) ;
          int state = Load( NameArray, &Service ) ;
          if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
          {
            fprintf( Out, "\n#I -- -- -- Loading completed ..." ) ;
            fprintf( Out, "  %s", state ? "ERROR" : "OK" ) ;
          }
        }
      }
      if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
        fprintf( Out, "\n#I -- -- Leaving directory." ) ;
      closedir( DirHandle ) ;
    }
    Directory = strtok( NULL, ":" ) ;
  }
  int Count = Registry.size() - OldCount ;
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
  {
    fprintf( Out, "\n#I -- Scan completed, new " ) ;
    fprintf( Out, "entities:  %i", Count ) ;
  }
  return ( Count > 0 ) ? 0 : HBEM_NO_PLUGINS_LOADED ;
}



int DomainData::Load( const char *Name, const Service_t *Service )
{
  BEM_Plugin *Handle = new BEM_Plugin( Name ) ;
  int state = Handle -> state() ;
  if ( state == 0 )
  {
    state = Handle -> init( Service ) ;
    if ( state == 0 )
    {
      std::shared_ptr<BEM_Plugin> NewPlugin( Handle ) ;
      Module.push_back( NewPlugin ) ;
      return 0 ;
    }
  }
  delete Handle ;
  return state ;
}



int DomainData::Create( BEM_Function *&f, const char *Type )
{
  const auto &E = Registry.find( Type ) ;
  if ( E == Registry.end() )  return HBEM_TYPE_NOT_REGISTERED ;
  Setup_t Data ;
  Data.Type = Type ;
  Data.State = 0 ;
  Data.Service = &Service ;
  f = (BEM_Function*) E -> second.Constructor( &Data ) ;
  return Data.State ;
}



int DomainData::Discard( const BEM_Function *f )
{
  if ( f == NULL )  return 0 ;
  const char *Type = f -> QueryType() ;
  const auto &E = Registry.find( Type ) ;
  if ( E == Registry.end() )  return HBEM_TYPE_NOT_REGISTERED ;
  Setup_t Data ;
  Data.Type = Type ;
  Data.State = 0 ;
  Data.Service = &Service ;
  E -> second.Destructor( f, &Data ) ;
  return Data.State ;
}



DomainData::~DomainData( void ) { Unload() ; }



int DomainData::Unload( void )
{
  int state = 0 ;
  for ( auto M = Module.begin() ; M != Module.end() ; ++ M )
    state |= (*M) -> exit() ;
  Module.clear() ;
  Registry.clear() ;
  return state ;
}



int DomainData::Read( const char *File, const char *Path )
{
  int state = HBEM_FAILED_TO_OPEN_FILE ;
  FILE *Input = NULL ;
  char *FileName = NULL ;
  char *buffer = NULL ;
  int Index = 0 ;
  int DomainIndex = 0 ;
  Line2D Segment ;
  Boundary2D Border ;
  Domain2D Patch ;
  if ( Open( Input, FileName, File, "r", Path ) == 0 )
  {
    size_t length = 0 ;
    ssize_t nread = 0 ;
    int step = 0 ;
    while ( ( nread = getline( &buffer, &length, Input ) ) != -1 )
    {
      state = HBEM_INVALID_DOMAIN_FILE ;
      if ( nread == 0 )  break ;
      buffer[ nread - 1 ] = '\0' ;
      if ( step == 0 && strcmp( buffer, ".type:" ) )  break ;
      if ( step == 1 && strcmp( buffer, "hbem-domain-data" ) )  break ;
      if ( step == 2 && strcmp( buffer, ".format:" ) )  break ;
      if ( step == 3 )
      {
        state = HBEM_WRONG_FILE_FORMAT ;
        if ( strcmp( buffer, HBEM_DOMAIN_FILE_FORMAT ) )  break ;
      }
      else if ( step == 4 && strcmp( buffer, ".name:" ) )  break ;
      else if ( step == 5 )
      {
        state = HBEM_DOMAIN_DATA_MISMATCH ;
        if ( Name.empty() )  Name = buffer ;
        if ( strcmp( buffer, Name.c_str() ) )  break ;
      }
      else if ( step == 6 && strcmp( buffer, ".created:" ) )  break ;
      else if ( step == 8 && strcmp( buffer, ".points:" ) )  break ;
      else if ( step == 9 )
      {
        if ( strcmp( buffer, ".segment:" ) )
        {
          if ( strcmp( buffer, ".domain:" ) )
          {
            if ( strcmp( buffer, ".end" ) )
            {
              state = HBEM_INVALID_POINT_DATA ;
              double x, y ;
              int n = sscanf( buffer, "%i %le %le", &Index, &x, &y ) ;
              if ( n < 3 || Index < 1 )  break ;
              state = HBEM_DUPLICATE_POINT_DATA ;
              if ( Points.count( Index ) > 0 )  break ;
              Points[ Index ] = { x, y } ;
              continue ;
            }
            state = 0 ;
            break ;
          }
          step = 14 ;          // Continue with domain data ...
        }
      }
      else if ( step == 10 )
      {
        state = HBEM_INVALID_SEGMENT_DATA ;
        Segment.Vertices.clear() ;
        int n = sscanf( buffer, "%i", &Index ) ;
        if ( n < 1 || Index < 1 )  break ;
        state = HBEM_DUPLICATE_SEGMENT_DATA ;
        if ( Segments.count( Index ) > 0 )  break ;
      }
      else if ( step == 11 && strcmp( buffer, ".material:" ) )  break ;
      else if ( step == 12 )
      {
        int n = sscanf( buffer, "%i", &Segment.Material ) ;
        if ( n < 1 )  break ;
      }
      else if ( step == 13 && strcmp( buffer, ".vertices:" ) )  break ;
      else if ( step == 14 )
      {
        if ( strcmp( buffer, ".segment:" ) )
        {
          if ( strcmp( buffer, ".domain:" ) )
          {
            if ( strcmp( buffer, ".end" ) )
            {
              char *token = strtok( buffer, " " ) ;
              while ( token )
              {
                state = HBEM_INVALID_VERTEX_DATA ;
                int p, n = sscanf( token, "%i", &p ) ;
                if ( n < 1 || p < 1 )  break ;
                state = HBEM_MISSING_VERTEX_DATA ;
                if ( Points.count( p ) < 1 )  break ;
                Segment.Vertices.push_back( p ) ;
                token = strtok( NULL, " " ) ;
              }
              if ( token )  break ;  // Error inside the inner loop!
              continue ;
            }
            step = 100 ;            // Ensure the parser terminates!
            state = 0 ;
          }
          step += 5 ;             // Proceed to reading domain data
        }
        Segments[ Index ] = Segment ;
        step -= 5 ;                           // Return to state 10
      }
      else if ( step == 15 )
      {
        state = HBEM_INVALID_DOMAIN_DATA ;
        int n = sscanf( buffer, "%i", &DomainIndex ) ;
        if ( n < 1 || DomainIndex < 1 )  break ;
        state = HBEM_DUPLICATE_DOMAIN_DATA ;
        if ( Patches.count( DomainIndex ) > 0 )  break ;
        DomainId.push_back( DomainIndex ) ;
        Patch.Boundaries.clear() ;
      }
      else if ( step == 16 && strcmp( buffer, ".constant:" ) )  break ;
      else if ( step == 17 )
      {
        state = HBEM_INVALID_DOMAIN_DATA ;
        int n = sscanf( buffer, "%le", &Patch.epsilon ) ;
        if ( n < 1 )  break ;
      }
      else if ( step == 18 && strcmp( buffer, ".boundary:" ) )  break ;
      else if ( step == 19 )
      {
        state = HBEM_INVALID_BOUNDARY_DATA ;
        int n = sscanf( buffer, "%i", &Index ) ;
        if ( n < 1 || Index < 1 )  break ;
        state = HBEM_DUPLICATE_BOUNDARY_DATA ;
        if ( Patch.Boundaries.count( Index ) > 0 )  break ;
        Border.f.clear() ;
        Border.BC.clear() ;
        Border.Segments.clear() ;
      }
      else if ( step == 20 )
      {
        if ( strcmp( buffer, ".evaluate:" ) )
        {
          state = HBEM_MISSING_BOUNDARY_DATA ;
          if ( strcmp( buffer, ".segments:" ) )  break ;
          step += 2 ;
        }
      }
      else if ( step == 21 )
      {
        state = HBEM_INVALID_BOUNDARY_DATA ;
        char *token = strtok( buffer, " " ) ;
        if ( token == NULL )  break ;
        Border.f = token ;
      }
      else if ( step == 22 && strcmp( buffer, ".segments:" ) )  break ;
      else if ( step == 23 )
      {
        if ( strcmp( buffer, ".materials:" ) )
        {
          if ( strcmp( buffer, ".boundary:" ) )
          {
            if ( strcmp( buffer, ".domain:" ) )
            {
              if ( strcmp( buffer, ".end" ) )
              {
                char *token = strtok( buffer, " " ) ;
                while ( token )
                {
                  state = HBEM_INVALID_SEGMENT_DATA ;
                  int p, n = sscanf( token, "%i", &p ) ;
                  if ( n < 1 )  break ;
                  state = HBEM_MISSING_SEGMENT_DATA ;
                  if ( Segments.count( abs(p) ) < 1 )  break ;
                  Border.Segments.push_back( p ) ;
                  token = strtok( NULL, " " ) ;
                }
                if ( token )  break ;   // Error inside the inner loop!
                continue ;
              }
              step = 100 ;             // Ensure the parser terminates!
              state = 0 ;
            }
            step -= 4 ;  // Return to state 15 to read new domain data
          }
          step -= 5 ;                            // Return to state 19
          Patch.Boundaries[ Index ] = Border ;
          Patches[ DomainIndex ] = Patch ;
        }
      }
      else if ( step == 24 )
      {
        if ( strcmp( buffer, ".boundary:" ) )
        {
          if ( strcmp( buffer, ".domain:" ) )
          {
            if ( strcmp( buffer, ".end" ) )
            {
              double c0, c1, cr ;
              state = HBEM_INVALID_MATERIAL_DATA ;
              int m, n = sscanf( buffer, "%i%lf%lf%lf", &m, &c0, &c1, &cr ) ;
              if ( n < 4 )  break ;
              state = HBEM_DUPLICATE_MATERIAL_DATA ;
              if ( Border.BC.count( m ) > 0 )  break ;
              Border.BC[ m ] = { c0, c1, cr } ;
              continue ;
            }
            step = 100 ;              // Ensure the parser terminates!
            state = 0 ;
          }
          step -= 4 ;    // Return to state 15 to read new domain data
        }
        Patch.Boundaries[ Index ] = Border ;
        Patches[ DomainIndex ] = Patch ;
        step -= 6 ;                              // Return to state 19
      }
      if ( ++ step > 24 )  break ;
    }
    free( buffer ) ;
    free( FileName ) ;
    fclose( Input ) ;
  }
  return state ;
}



int DomainData::Write( const char *File ) const
{
  time_t utc = time( NULL ) ;
  struct tm *gm = gmtime( &utc ) ;
  char Time[ 128 ] ;
  strftime( Time, 128, "%x -- %T", gm ) ;
  FILE *Out = fopen( File, "w" ) ;
  if ( Out )
  {
    fprintf( Out, ".type:\nhbem-domain-data\n.format:\n" ) ;
    fprintf( Out, "%s\n.name:", HBEM_DOMAIN_FILE_FORMAT ) ;
    fprintf( Out, "\n%s", Name.c_str() ) ;
    fprintf( Out, "\n.created:\n%s\n.points:", Time ) ;
    for ( const auto &p : Points )
    {
      fprintf( Out, "\n%5i  %+16.12e", p.first, p.second.x ) ;
      fprintf( Out, "  %+16.12e", p.second.y ) ;
    }
    for ( const auto &s : Segments )
    {
      fprintf( Out, "\n.segment:\n%i\n.material:", s.first ) ;
      fprintf( Out, "\n%i\n.vertices:", s.second.Material ) ;
      int n = -1 ;
      for ( int v : s.second.Vertices )
      {
        fprintf( Out, ++ n % 4 ? "  %5i" : "\n%5i", v ) ;
      }
    }
    for ( const auto &p : Patches )
    {
      fprintf( Out, "\n.domain:\n%i\n.constant:", p.first ) ;
      fprintf( Out, "\n%+16.12e", p.second.epsilon ) ;
      for ( const auto &b : p.second.Boundaries )
      {
        fprintf( Out, "\n.boundary:\n%i", b.first ) ;
        if ( !b.second.f.empty() )
          fprintf( Out, "\n.evaluate:\n%s", b.second.f.c_str() ) ;
        fprintf( Out, "\n.segments:" ) ;
        int n = -1 ;
        for ( int s : b.second.Segments )
          fprintf( Out, ++ n % 4 ? "  %5i" : "\n%5i", s ) ;
        if ( b.second.BC.empty() )  continue ;
        fprintf( Out, "\n.materials:" ) ;
        for ( const auto &bc : b.second.BC )
        {
          fprintf( Out, "\n%5i    ", bc.first ) ;
          fprintf( Out, "%+16.12e  ", bc.second.c0 ) ;
          fprintf( Out, "%+16.12e  ", bc.second.c1 ) ;
          fprintf( Out, "  %+16.12e", bc.second.cr ) ;
        }
      }
    }
    fprintf( Out, "\n.end\n" ) ;
    fclose( Out ) ;
    return 0 ;
  }
  return HBEM_FAILED_TO_WRITE_FILE ;
}



int DomainData::Start( void )
{
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
  {
    FILE *Out = Log ? Log : stdout ;
    time_t utc = time( NULL ) ;
    struct tm *gm = gmtime( &utc ) ;
    char Time[ 128 ] ;
    strftime( Time, 128, "%x -- %T", gm ) ;
    fprintf( Out, "#I Start reading BEM domain data" ) ;
    fprintf( Out, "  [ %s ]", Time ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::Stop( void )
{
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
  {
    FILE *Out = Log ? Log : stdout ;
    fprintf( Out, "\n#I Done reading BEM domain data." ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::Search( const char *Path )
{
  if ( Path )
  {
    ModulePath += ':' ;
    ModulePath += Path ;
  }
  else
  {
    const char *env = getenv( HBEM_SHELL_ENV_SEARCHPATH ) ;
    ModulePath.clear() ;
    if ( Path )  ModulePath = env ;
  }
  if ( ModulePath.length() < HBEM_MAX_FILE_PATH_LENGTH )
    return 0 ;
  return HBEM_ILLEGAL_SEARCHPATH ;
}



int DomainData::SetLog( FILE *LogFile )
{
  if ( Log )
  {
    if ( Log == LogFile )  return 0 ;
    fclose( Log ) ;
  }
  Log = LogFile ;
  return 0 ;
}



int DomainData::LogInfo( const char *Message, int nesting ) const
{
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
  {
    FILE *Out = Log ? Log : stdout ;
    fprintf( Out, "\n#I" ) ;
    for ( int i = 0 ; i < nesting ; i++ )  fprintf( Out, " --" ) ;
    fprintf( Out, " %s", Message ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::LogInfo( std::stringstream &msg, int nesting ) const
{
  if ( LogLevel > HBEM_NO_PROGRESS_REPORTS )
  {
    FILE *Out = Log ? Log : stdout ;
    fprintf( Out, "\n#I" ) ;
    for ( int i = 0 ; i < nesting ; i++ )  fprintf( Out, " --" ) ;
    fprintf( Out, " %s", msg.str().c_str() ) ;
    msg.str( "" ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::LogAlert( const char *Message, int nesting ) const
{
  if ( LogLevel > HBEM_NO_WARNING_MESSAGES )
  {
    FILE *Out = Log ? Log : stdout ;
    fprintf( Out, "\n#A" ) ;
    for ( int i = 0 ; i < nesting ; i++ )  fprintf( Out, " --" ) ;
    fprintf( Out, " %s", Message ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::LogAlert( std::stringstream &msg, int nesting ) const
{
  if ( LogLevel > HBEM_NO_WARNING_MESSAGES )
  {
    FILE *Out = Log ? Log : stdout ;
    fprintf( Out, "\n#A" ) ;
    for ( int i = 0 ; i < nesting ; i++ )  fprintf( Out, " --" ) ;
    fprintf( Out, " %s", msg.str().c_str() ) ;
    msg.str( "" ) ;
    return 0 ;
  }
  return HBEM_LOGLEVEL_TOO_LOW ;
}



int DomainData::ChangeLogLevel( int NewLevel )
{
  int OldLevel = LogLevel ;
  LogLevel = NewLevel ;
  return OldLevel ;
}



int DomainData::Update( const char *Name, const Plugin_t *Data )
{
  int Level = -1 ;
  if ( Data -> Version < MaxVersion )
  {
    const char *Message = "-- -- -- -- Updating entity:     " ;
    if ( Registry.count( Name ) > 0 )
    {
      auto &Entity = Registry[ Name ] ;
      if ( Entity.Version < Data -> Version )
      {
        Entity = *Data ;
        Level = LogLevel ;
      }
    }
    else
    {
      Message = "-- -- -- -- Registering entity:  " ;
      Level = LogLevel ;
      Registry[ Name ] = *Data ;
    }
    if ( Level > HBEM_NO_PROGRESS_REPORTS )
    {
      FILE *Out = Log ? Log : stdout ;
      fprintf( Out, "\n#I %s%s", Message, Name ) ;
      fprintf( Out, "  [%i", Data -> Version.Major ) ;
      fprintf( Out, ".%i]", Data -> Version.Minor ) ;
    }
    return 0 ;
  }
  return HBEM_UNSUPPORTED_PLUGIN ;
}


int DomainData::CreateMesh( MeshData &Mesh, double MeshWidth )
{
  if ( MeshWidth > 1e-8 )
  {
    std::stringstream msg ;
    Container VertexData ;
    VertexData.MeshWidth = MeshWidth ;
    VertexData.DOFIndexCounter = 0 ;
    VertexData.CurrentDOFIndex = 0 ;
    VertexData.f0 = NULL ;
    VertexData.State = 0 ;
    Mesh.Clear() ;
    Mesh.SetName( Name ) ;
    int state ;
    for ( const auto &P : Patches )
    {
      msg << "Start processing patch:  " << P.first ;
      LogInfo( msg, 1 ) ;
      VertexData.CurrentDomain = P.first ;
      VertexData.epsilon = P.second.epsilon ;
      for ( const auto &B : P.second.Boundaries )
      {
        msg << "Start tracing boundary:  " << B.first ;
        LogInfo( msg, 2 ) ;
        VertexData.StartNewBoundary = true ;
        VertexData.CurrentBoundary = B.first ;
        if ( B.second.f.size() )
        {
          state = Discard( VertexData.f0 ) ;
          if ( state )  return state ;
          state = Create( VertexData.f0, B.second.f.c_str() ) ;
          if ( state )
          {
            msg << "Requested entity unavailble:  " << B.second.f ;
            LogAlert( msg, 3 ) ;
            LogAlert( "Ignoring boundary conditions ...", 3 ) ;
            VertexData.f0 = NULL ;
          }
          else
          {
            PluginVersion_t rev = VertexData.f0 -> Revision() ;
            msg << "Boundary conditions, entity:  " << B.second.f ;
            msg << "  [" << rev.Major << "." << rev.Minor << "]" ;
            LogInfo( msg, 3 ) ;
          }
        }
        for ( int s : B.second.Segments )
        {
          VertexData.CurrentSegment = abs(s) ;
          VertexData.StartNewSegment = true ;
          const Line2D &Section = Segments[ abs(s) ] ;
          VertexData.CurrentMaterial = Section.Material ;
          msg << "Entering segment:  " << abs(s) ;
          LogInfo( msg, 3 ) ;
          RobinBC bc0 = { 0.0, 0.0, 0.0 } ;
          const auto BC0 = B.second.BC.find( Section.Material ) ;
          if ( BC0 != B.second.BC.end() )
          {
            bc0 = BC0 -> second ;
            msg << "Boundary conditions, material:  " << Section.Material ;
            LogInfo( msg, 4 ) ;
            msg << "Coefficient 0:  " << bc0.c0 ;
            LogInfo( msg, 4 ) ;
            msg << "Coefficient 1:  " << bc0.c1 ;
            LogInfo( msg, 4 ) ;
            msg << "Coefficient R:  " << bc0.cr ;
            LogInfo( msg, 4 ) ;
          }
          VertexData.BoundaryCondition = bc0 ;
          VertexData.MinimalWidth = DBL_MAX ;
          VertexData.MaximalWidth = -1 ;
          uint64_t DOFStart = VertexData.CurrentDOFIndex ;
          if ( s < 0 )
          {
            auto v = Section.Vertices.rbegin() ;
            auto vend = Section.Vertices.rend() ;
            for ( ; v != vend ; ++ v )
            {
              VertexData.CurrentVertex = *v ;
              int error = TraceBoundary( Mesh, VertexData ) ;
              if ( error )  return error ;
              VertexData.LastVertex = VertexData.CurrentVertex ;
            }
          }
          else
          {
            auto v = Section.Vertices.begin() ;
            auto vend = Section.Vertices.end() ;
            for ( ; v != vend ; ++ v )
            {
              VertexData.CurrentVertex = *v ;
              int error = TraceBoundary( Mesh, VertexData ) ;
              if ( error )  return error ;
              VertexData.LastVertex = VertexData.CurrentVertex ;
            }
          }
          uint64_t DOF = VertexData.CurrentDOFIndex - DOFStart ;
          msg << "Minimal panel width:  " << VertexData.MinimalWidth ;
          LogInfo( msg, 4 ) ;
          msg << "Maximal panel width:  " << VertexData.MaximalWidth ;
          LogInfo( msg, 4 ) ;
          msg << "Panels processed:  " << DOF / 2 ;
          LogInfo( msg, 4 ) ;
          VertexData.LastSegment = abs(s) ;
          LogInfo( "Leaving segment.", 3 ) ;
        }
        if ( VertexData.FirstVertex != VertexData.CurrentVertex )
        {
          LogAlert( "Boundary not closed!", 3 ) ;
          LogAlert( "Matrix assembly failure likely ...", 3 ) ;
          VertexData.State = HBEM_BOUNDARY_NOT_CLOSED ;
        }
        LogInfo( "Done tracing boundary.", 2 ) ;
      }
      LogInfo( "Done processing patch.", 1 ) ;
    }
    if ( Patches.size() > 0 )
    {
      LogInfo( "Identifying domain interfaces ...", 1 ) ;
      if ( Mesh.MatchPanel() )
      {
        LogInfo( "Boundary conditions ...   OK", 1 ) ;
        return state ? state : VertexData.State ;
      }
      LogInfo( "Boundary conditions ...   NOK", 1 ) ;
      return HBEM_BOUNDARY_CONDITIONS_UNMET ;
    }
    LogAlert( "No BEM domain data available ...", 1 ) ;
    return HBEM_NO_DOMAIN_DATA_READ ;
  }
  return HBEM_MESH_WIDTH_TOO_SMALL ;
}


int DomainData::TraceBoundary( MeshData &Mesh, Container &VertexData )
{
  std::stringstream msg ;
  PointData P0 ;
  EdgeData E0 ;
  bool bcvalid = fabs( VertexData.BoundaryCondition.c0 ) > 1e-4 ||
                 fabs( VertexData.BoundaryCondition.c1 ) > 1e-4 ;
  if ( VertexData.StartNewBoundary )
  {
    VertexData.StartNewBoundary = false ;
    VertexData.FirstVertex = VertexData.CurrentVertex ;
    VertexData.LastVertex = VertexData.FirstVertex ;
    VertexData.ArcLength = 0.0 ;
  }
  if ( VertexData.StartNewSegment )
  {
    VertexData.StartNewSegment = false ;
    if ( VertexData.LastVertex == VertexData.CurrentVertex )
      return 0 ;
    msg << "Gap between segments " << VertexData.LastSegment ;
    msg << " and " << VertexData.CurrentSegment  << " ..." ;
    LogAlert( msg, 4 ) ;
    VertexData.State = HBEM_DISCONNECTED_SEGMENTS ;
  }
  const Point2D p0 = Points[ VertexData.LastVertex ] ;
  const Point2D p1 = Points[ VertexData.CurrentVertex ] ;
  const double length = sqrt( ( p0.x - p1.x ) * ( p0.x - p1.x ) +
                              ( p0.y - p1.y ) * ( p0.y - p1.y ) ) ;
  uint64_t Number = FLT_EPSILON + length / VertexData.MeshWidth ;
  Number = Number > 1 ? Number : 2 ;
  VertexData.DOFIndexCounter += 2 * Number ;
  if ( VertexData.DOFIndexCounter > HBEM_SIZE_THRESHOLD )
    return HBEM_SIZE_LIMIT_EXCEEDED ;
  const double dx = ( p1.x - p0.x ) / Number ;
  const double dy = ( p1.y - p0.y ) / Number ;
  double x0 = p0.x + 0.5 * dx ;
  double y0 = p0.y + 0.5 * dy ;
  const double nx = ( p1.y - p0.y ) / length ;
  const double ny = ( p0.x - p1.x ) / length ;
  const double width = length / Number ;
  double ArcLength = VertexData.ArcLength + 0.5 * width ;
  if ( width < VertexData.MinimalWidth )  VertexData.MinimalWidth = width ;
  if ( width > VertexData.MaximalWidth )  VertexData.MaximalWidth = width ;
  E0.domain = VertexData.CurrentDomain ;
  E0.boundary = VertexData.CurrentBoundary ;
  E0.arclen = VertexData.ArcLength ;
  E0.start = p0 ;
  E0.end = p1 ;
  E0.normal = { nx, ny } ;
  Mesh.AddEdge( E0 ) ;
  P0.interface = false ;
  P0.match = 0 ;
  P0.epsilon = VertexData.epsilon ;
  P0.epsilon_m = VertexData.epsilon ;
  P0.panelsize = width ;
  P0.segment = VertexData.CurrentSegment ;
  P0.normal = { nx, ny } ;
  P0.material = VertexData.CurrentMaterial ;
  P0.domain = VertexData.CurrentDomain ;
  P0.boundary = VertexData.CurrentBoundary ;
  if ( VertexData.f0 )
  {
    for ( uint64_t j = 0 ; j < Number ; j ++ )
    {
      P0.index = VertexData.CurrentDOFIndex ;
      P0.midpoint = { x0, y0 } ;
      P0.arclen = ArcLength ;
      int state = VertexData.f0 -> Evaluate( P0 ) ;
      if ( state )  return state ;
      Mesh.AddPanel( P0 ) ;
      x0 += dx ;
      y0 += dy ;
      ArcLength += width ;
      VertexData.CurrentDOFIndex += 2 ;
    }
  }
  else
  {
    P0.c0 = VertexData.BoundaryCondition.c0 ;
    P0.c1 = VertexData.BoundaryCondition.c1 ;
    P0.cr = VertexData.BoundaryCondition.cr ;
    P0.valid_bc = bcvalid ;
    for ( uint64_t j = 0 ; j < Number ; j ++ )
    {
      P0.index = VertexData.CurrentDOFIndex ;
      P0.midpoint = { x0, y0 } ;
      P0.arclen = ArcLength ;
      Mesh.AddPanel( P0 ) ;
      x0 += dx ;
      y0 += dy ;
      ArcLength += width ;
      VertexData.CurrentDOFIndex += 2 ;
    }
  }
  VertexData.ArcLength += length ;
  return 0 ;
}
