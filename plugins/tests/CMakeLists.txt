
# This cmake file collects all subdirectories whose names are simply
# numbers in the range 0-999 and creates a test for each of the
# directories it has encountered.

FILE( GLOB TEST_DIRECTORIES [0-9][0-9][0-9] )

# The variable TEST_DIRECTORIES stores a list of all directories which
# may contain a test driver for some unit test. These are all included
# in the list of subdirectories to be processed.

FOREACH( DIR ${TEST_DIRECTORIES} )
  GET_FILENAME_COMPONENT( TAIL ${DIR} NAME )
  IF( EXISTS ${DIR}/CMakeLists.txt )
    SUBDIRS( ${TAIL} )
  ENDIF( EXISTS ${DIR}/CMakeLists.txt )
ENDFOREACH(DIR)

# On Unix system which use GNU development tools to compile the source
# code test coverage information can be obtained from the build with the
# help of a tool called "gcov". To collect such coverage information
# certain compiler switches must be used during code generation and the
# resulting binary must be linked against a "gcov" library.

IF( UNIX AND ENABLE_TEST_COVERAGE )
  ADD_DEFINITIONS( -g -fprofile-arcs -ftest-coverage )
ENDIF( UNIX AND ENABLE_TEST_COVERAGE )

# On Unix system which use GNU development tools to compile the source
# code profiling information can be obtained from the executable with the
# help of a tool called "gprof". To collect such profiling information
# certain compiler switches must be used during code generation. On top
# of that the linker switch "-pg" must be used.

IF( UNIX AND ENABLE_CODE_PROFILING )
  ADD_DEFINITIONS( -pg )
ENDIF( UNIX AND ENABLE_CODE_PROFILING )

# The directory below contains those header files which constitute the
# interface of the current project. The unit tests will certainly need
# access to them, hence the directory is included into the search path
# by default on this level of the source tree. Using other projects is
# discouraged, as each unit test should be self-contained.

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../include )
