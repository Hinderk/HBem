
INCLUDE( GenerateExportHeader )

###############################################################################
#
# Define the source files which must be compiled into shared objects ...
#
  FILE( GLOB SOURCES "*.cpp" )
#
###############################################################################
#
  FILE( MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/modules" )

  SET( CMAKE_CXX_VISIBILITY_PRESET "hidden" )
  SET( CMAKE_VISIBILITY_INLINES_HIDDEN ON )

  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
  INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/../include )

  SET( SO_VERSION "1" )
  SET( MINOR_VERSION "0.0" )

  SET( LIB_VERSION ${SO_VERSION}.${MINOR_VERSION} )

  SET( BUILD_SHARED_LIBS ON )

  FOREACH( FILE ${SOURCES} )
    GET_FILENAME_COMPONENT( FILENAME ${FILE} NAME )
    STRING( REGEX REPLACE ".cpp$" "" TRUNK ${FILENAME} )
    STRING( TOUPPER ${TRUNK} LIBNAME )
    ADD_LIBRARY( ${LIBNAME} ${FILE} )
    TARGET_LINK_LIBRARIES( ${LIBNAME} HBEM::HBEM )

    ADD_CUSTOM_COMMAND(
      TARGET ${LIBNAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${LIBNAME}> ${CMAKE_BINARY_DIR}/modules
    )

    SET_TARGET_PROPERTIES( ${LIBNAME} PROPERTIES
      VERSION ${LIB_VERSION}
      SOVERSION ${SO_VERSION}
    )

    GENERATE_EXPORT_HEADER(
      ${LIBNAME}
      EXPORT_FILE_NAME "export_${TRUNK}.h"
    )

  ENDFOREACH( FILE )
#
###############################################################################
