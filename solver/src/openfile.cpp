
#include "openfile.h"

#include <string.h>
#ifdef _WIN32
  #include "windows/dirent.h"
#else
  #include <dirent.h>
#endif
#include <stdio.h>
#include <stdlib.h>



int Open( FILE       *&Handle    ,        // handle for the opened file
          char       *&FullName  ,        // fully qualified file name
          const char *Name       ,        // file name as requested
          const char *Mode       ,        // file open mode
          const char *SearchPath )        // search path for input files
{
  Handle = NULL ;
  FullName = NULL ;
  if ( strlen( Name ) > _MAX_FILE_PATH_LENGTH )
    return _ABS_FILENAME_TOO_LONG ;
  char DirArray[ 2 * _MAX_FILE_PATH_LENGTH + 1 ] ;
  char FileName[ _MAX_FILE_PATH_LENGTH + 1 ] ;
  char NameArray[ _MAX_FILE_PATH_LENGTH + 1 ] ;
  NameArray[ 0 ] = '\0' ;
  int LengthOfName = 0 ;
  char WorkingArray[ _MAX_FILE_PATH_LENGTH + 1 ] ;
  strcpy( WorkingArray, Name ) ;
  char *Prefix = strtok( WorkingArray, "/\\" ) ;
  if ( Prefix == WorkingArray )
  {
    char *File = strtok( NULL, "\\/" ) ;
    while ( File != NULL )
    {
      NameArray[ LengthOfName ++ ] = '/' ;
      strcpy( NameArray + LengthOfName, Prefix ) ;
      LengthOfName += strlen( Prefix ) ;
      Prefix = File ;
      File = strtok( NULL, "\\/" ) ;
    }
    strcpy( FileName + 1, Prefix ) ;
    FileName[ 0 ] = '/' ;
    if ( SearchPath == NULL )
      SearchPath = getenv( _SHELL_ENV_SEARCHPATH ) ;
    if ( SearchPath == NULL )
    {
      WorkingArray[ 0 ] = '.' ;
      strcpy( WorkingArray + 1, NameArray ) ;
      strcpy( WorkingArray + LengthOfName + 1, FileName ) ;
      Handle = fopen( WorkingArray, Mode ) ;
      if ( Handle )
      {
        FullName = strdup( WorkingArray ) ;
        return 0 ;
      }
    }
    else
    {
      if ( strlen( SearchPath ) > _MAX_FILE_PATH_LENGTH )
        return _ILLEGAL_SEARCHPATH ;
      strcpy( WorkingArray, SearchPath ) ;
      char *CurrentDirectory = strtok( WorkingArray, ":" ) ;
      while ( CurrentDirectory != NULL )
      {
        int DirLength = strlen( CurrentDirectory ) ;
        strcpy( DirArray, CurrentDirectory ) ;
        strcpy( DirArray + DirLength, NameArray ) ;
        DIR *DirHandle = opendir( DirArray ) ;
        if ( DirHandle != NULL )
        {
          struct dirent *DirEntry = readdir( DirHandle ) ;
          while ( DirEntry != NULL )
          {
            if ( ! strcmp( FileName + 1, DirEntry -> d_name ) )
            {
              DirLength += LengthOfName ;
              strcpy( DirArray + DirLength, FileName ) ;
              closedir( DirHandle ) ;
              Handle = fopen( DirArray, Mode ) ;
              if ( Handle )
              {
                FullName = strdup( DirArray ) ;
                return 0 ;
              }
              return _FAILED_TO_OPEN_FILE ;
            }
            DirEntry = readdir( DirHandle ) ;
          }
          closedir( DirHandle ) ;
        }
        CurrentDirectory = strtok( NULL, ":" ) ;
      }
    }
  }
  else
  {
    Handle = fopen( Name, Mode ) ;
    if ( Handle )
    {
      FullName = strdup( Name ) ;
      return 0 ;
    }
  }
  return _FAILED_TO_OPEN_FILE ;
}
