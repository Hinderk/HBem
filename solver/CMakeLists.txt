
# Specify the name of the executable to be built by the project:

SET( SOLVER "hbem" )

# Specifiy the name of the object library used to compile & link
# the various test drivers:

SET( LIBNAME "BEMLIB" )

# Create an interface target for the build system ...

SET( HEADERS "HBEM" )

ADD_LIBRARY( ${HEADERS} INTERFACE )
ADD_LIBRARY( ${HEADERS}::${HEADERS} ALIAS ${HEADERS} )

TARGET_INCLUDE_DIRECTORIES(
  ${HEADERS}
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Define the folders containing the source code of the solver.

ADD_SUBDIRECTORY( "src" )
ADD_SUBDIRECTORY( "tests" )
