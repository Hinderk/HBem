
#include <stdlib.h>
#include <memory>
#include <iostream>

#include "domaindata.h"
#include "meshdata.h"
#include "cmdline/optionparser.h"



int main( int argc, const char *argv[] )
{
  std::shared_ptr<OptionParser> Opts( CreateOptionParser( argc, argv ) ) ;
  int LOGLVL = Opts -> AddOption( 16, "-v", "--verbosity", "level" ) ;
  int OUTPUT = Opts -> AddOption( "hbem.dat", "-o", "--output", "file" ) ;
  int INPUT = Opts -> AddOption( "", "-i", "--input", "file" ) ;
  int LOGFILE = Opts -> AddOption( "", "-l", "--log", "file" ) ;
  int PLUGIN = Opts -> AddOption( "", "-m", "--module", "path" ) ;
  int SEARCH = Opts -> AddOption( "", "-s", "--search", "path" ) ;
  int USAGE = Opts -> AddOption( false, "-?", "--help" ) ;
  Opts -> UseDefault( LOGLVL, true ) ;
  Opts -> UseDefault( OUTPUT, true ) ;
  int State = Opts -> Parse() ;
  OptionValue Opt ;
  if ( State || Opts -> QueryOption( Opt, USAGE ) )
  {
    char Buffer[ 256 ], Output[ 256 ] ;
    Opts -> Usage( Buffer, 256 ) ;
    Opts -> Prettify( Output, Buffer, 256 ) ;
    fprintf( stdout, "%s\n", Output ) ;
    return 0 ;
  }
  Opts -> QueryOption( Opt, LOGLVL ) ;
  FILE *Out = NULL ;
  DomainData BEMData ;
  BEMData.ChangeLogLevel( Opt ) ;
  if ( Opts -> QueryOption( Opt, LOGFILE ) )
  {
    Out = fopen( Opt, "a+" ) ;
    BEMData.SetLog( Out ) ;
  }
  FILE *Log = Out ? Out : stdout ;
  BEMData.Start() ;
  int counter = Opts -> QueryOption( Opt, PLUGIN ) ;
  std::string ModulePath ;
  while ( counter > 0 )
  {
    ModulePath += ":" ;
    ModulePath += (const char *) Opt ;
    counter = Opts -> NextOption( Opt ) ;
  }
  BEMData.Load( ModulePath.c_str() ) ;
  counter = Opts -> QueryOption( Opt, SEARCH ) ;
  while ( counter > 0 )
  {
    int error = BEMData.Search( Opt ) ;
    if ( error > 0 )
    {
      fprintf( Log, "\n#E Search path too long -- Aborting." ) ;
      return error ;
    }
    counter = Opts -> NextOption( Opt ) ;
  }
  counter = Opts -> QueryOption( Opt, INPUT ) ;
  while ( counter > 0 )
  {
    BEMData.Read( Opt ) ;
    counter = Opts -> NextOption( Opt ) ;
  }
  MeshData M0 ;
  M0.CreateMesh( BEMData, 0.01 ) ;

  BEMData.Stop() ;

  // Compute Results ...

  Opts -> QueryOption( Opt, OUTPUT ) ;
  BEMData.Write( Opt ) ;
  fprintf( Log, "\n" ) ;
  if ( Out )  fclose( Out ) ;
  return 0 ;
}
