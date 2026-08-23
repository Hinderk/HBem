
#include <stdlib.h>
#include <memory>
#include <iostream>

#include "domaindata.h"
#include "stopwatch.h"
#include "meshdata.h"
#include "matrix.h"
#include "vector.h"
#include "cmdline/optionparser.h"

using namespace HBEM ;



int main( int argc, const char *argv[] )
{
  StopWatch T0 ;
  T0.Start() ;
  std::shared_ptr<OptionParser> Opts( CreateOptionParser( argc, argv ) ) ;
  int LOGLVL = Opts -> AddOption( 16, "-v", "--verbosity", "level" ) ;
  int OUTPUT = Opts -> AddOption( "", "-d", "--domain", "file" ) ;
  int VOLUME = Opts -> AddOption( "result.dat", "-o", "--output", "file" ) ;
  int INPUT = Opts -> AddOption( "", "-i", "--input", "file" ) ;
  int LOGFILE = Opts -> AddOption( "", "-l", "--log", "file" ) ;
  int PLUGIN = Opts -> AddOption( "", "-m", "--module", "path" ) ;
  int SEARCH = Opts -> AddOption( "", "-s", "--search", "path" ) ;
  int MESH = Opts -> AddOption( 0.05d, "-w", "--meshsize", "width" ) ;
  int WIDTH = Opts -> AddOption( 0.01d, "-p", "--panelsize", "length" ) ;
  int USAGE = Opts -> AddOption( false, "-?", "--help" ) ;
  Opts -> UseDefault( LOGLVL, true ) ;
  Opts -> UseDefault( VOLUME, true ) ;
  Opts -> UseDefault( MESH, true ) ;
  Opts -> UseDefault( WIDTH, true ) ;
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
      if ( Out )  fclose( Out ) ;
      return error ;
    }
    counter = Opts -> NextOption( Opt ) ;
  }
  counter = Opts -> QueryOption( Opt, INPUT ) ;
  while ( counter > 0 )
  {
    int error = BEMData.Read( Opt ) ;
    if ( error > 0 )
    {
      fprintf( Log, "\n#E Failed to parse input file -- Aborting." ) ;
      if ( Out )  fclose( Out ) ;
      return error ;
    }
    counter = Opts -> NextOption( Opt ) ;
  }
  BEMData.Stop() ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  T0.Continue() ;
  Opts -> QueryOption( Opt, WIDTH ) ;
  MeshData M0 ;
  fprintf( Log, "\n\n#I Creating the surface mesh ..." ) ;
  int error = BEMData.CreateMesh( M0, Opt ) ;
  if ( error > 0 )
  {
    fprintf( Log, "\n#E Mesh data inconsistent -- Aborting." ) ;
    if ( Out )  fclose( Out ) ;
    return error ;
  }
  fprintf( Log, "\n#I Mesh creation successful." ) ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  T0.Continue() ;
  fprintf( Log, "\n\n#I Assembling the BEM matrix ..." ) ;
  Matrix A ;
  Vector x, f ;
  error = M0.Assemble( A, f ) ;
  if ( error > 0 )
  {
    fprintf( Log, "\n#E Memory allocation problems -- Aborting." ) ;
    if ( Out )  fclose( Out ) ;
    return error ;
  }
  fprintf( Log, "\n#I Matrix assembly successful." ) ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  T0.Continue() ;
  fprintf( Log, "\n\n#I Computing the LR factorisation ..." ) ;
  error = A.Solve( x, f ) ;
  fprintf( Log, "\n#I Factorisation complete." ) ;
  fprintf( Log, "\n#I Linear BEM equations solved." ) ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  T0.Continue() ;
  fprintf( Log, "\n\n#I Recovering volume data ..." ) ;
  Opts -> QueryOption( Opt, MESH ) ;
  const double width = Opt ;
  fprintf( Log, "\n#I -- Mesh width: %g", width ) ;
  AreaData Area( width, width ) ;
  error = M0.ComputeAreaData( Area, x ) ;
  fprintf( Log, "\n#I Volume data reconstructed." ) ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  T0.Continue() ;
  fprintf( Log, "\n\n#I Storing the numerical results ..." ) ;
  Opts -> QueryOption( Opt, VOLUME ) ;
  fprintf( Log, "\n#I -- File name: %s", (const char *) Opt ) ;
  Area.Write( Opt, GNUPLOT ) ;
  fprintf( Log, "\n#I Output file created." ) ;
  fprintf( Log, "\n\n#I Elapsed time: %i ms", T0.RealTime() / 1000 ) ;
  T0.Stop() ;
  fprintf( Log, "\n\n#I Total elapsed time: %i ms\n", T0.RealTime()/1000 ) ;
  if ( Opts -> QueryOption( Opt, OUTPUT ) )  BEMData.Write( Opt ) ;
  if ( Out )  fclose( Out ) ;
  return 0 ;
}
