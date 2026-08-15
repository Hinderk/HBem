
#ifndef __GETLINE_H
#define __GETLINE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
  typedef long signed  ssize_t ;
#endif



static size_t getline( char **buf, size_t *size, FILE *fp )
{
  if ( *buf == NULL || *size == 0 )
  {
    *size = 1024 ;
    if ( ( *buf = (char*) malloc( *size ) ) == NULL )  return -1 ;
  }
  char *ptr = *buf ;
  char *end = *buf + *size ;
  while ( true )
  {
    int c = fgetc( fp ) ;
    if ( c == -1 )
    {
      if ( feof( fp ) )
        return ptr == *buf ? -1 : ptr - *buf ;
      return -1 ;
    }
    *ptr ++ = c ;
    if ( c == '\n' )
    {
      *ptr = '\0' ;
      return ptr - *buf ;
    }
    if ( ptr + 2 >= end )
    {
      char *nbuf ;
      size_t nsize = *size * 2 ;
      ssize_t d = ptr - *buf ;
      if ( ( nbuf = (char*) realloc( *buf, nsize ) ) == NULL )  return -1 ;
      *buf = nbuf ;
      *size = nsize ;
      end = nbuf + nsize ;
      ptr = nbuf + d ;
    }
  }
}


#endif      // __GETLINE_H
