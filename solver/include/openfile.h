
#ifndef __HBEM_OPENFILE_H
#define __HBEM_OPENFILE_H

#include <string>
#include <stdio.h>

#include "defines.h"


#define _MAX_FILE_PATH_LENGTH   HBEM_MAX_FILE_PATH_LENGTH

#define _FILE_SEARCHPATH        HBEM_SHELL_ENV_SEARCHPATH

#define _ABS_FILENAME_TOO_LONG  HBEM_ABS_FILENAME_TOO_LONG
#define _ILLEGAL_SEARCHPATH     HBEM_ILLEGAL_SEARCHPATH
#define _FAILED_TO_OPEN_FILE    HBEM_FAILED_TO_OPEN_FILE
#define _SHELL_ENV_SEARCHPATH   HBEM_SHELL_ENV_SEARCHPATH


int Open( FILE *&, char *&, const char *, const char *, const char * ) ;

#endif   // __HBEM_OPENFILE_H
