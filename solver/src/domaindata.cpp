
#include <cstring>
#include <fstream>
#include <iomanip>

#include "defines.h"
#include "pointdata.h"
#include "openfile.h"
#include "domaindata.h"

#ifdef _WIN32
  #include "windows/getline.h"
#endif



DomainData::DomainData( int Level ) :

  Log( NULL ) ,
  LogLevel( Level ) ,
  SearchPath() ,
  Name() ,
  Points() ,
  Segments() ,
  Patches() {}



void DomainData::Clear( void )
{
  SearchPath.clear() ;
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
              int m, n = sscanf( buffer, "%i %lf %lf %lf", &m, &c0, &c1, &cr ) ;
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
    fprintf( Out, "\n.created:\n%s\n.points", Time ) ;
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
    fprintf( Out, "\n.end" ) ;
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
    SearchPath += ':' ;
    SearchPath += Path ;
  }
  else
  {
    const char *env = getenv( HBEM_SHELL_ENV_SEARCHPATH ) ;
    SearchPath.clear() ;
    if ( Path )  SearchPath = env ;
  }
  if ( SearchPath.length() < HBEM_MAX_FILE_PATH_LENGTH )
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
