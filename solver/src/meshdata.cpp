
#include <cstring>
#include <fstream>
#include <iomanip>

#include "defines.h"
#include "domaindata.h"
#include "meshdata.h"



MeshData::MeshData() :

  Panel() ,
  epsilon() ,
  DomainStart() ,
  DomainEnd() {}



int MeshData::CreateMesh( const DomainData &D, double h )
{
  char buffer[ 256 ] ;
  int DoFCounter = 0 ;
  for ( const auto &P : D.Patches )
  {
    sprintf( buffer, "Start processing patch %i", P.first ) ;
    D.LogInfo( buffer, 1 ) ;
    for ( const auto &s : D.Segments )
    {

    }

    sprintf( buffer, "Done processing patch %i", P.first ) ;
    D.LogInfo( buffer, 1 ) ;
  }

  return 0 ;
}
