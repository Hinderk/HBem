###############################################################################
#
# Define the source files which must be compiled into the test driver...
#
  SET( SOURCES "main.cpp" )
#
###############################################################################
#
# The following code is completely generic and may be recycled for all unit
# test in the project. The test setup is performed in the module "TestSetup"
# which is supposed to reside in the current directory. The CMake module will
# define the following variables: TEST_NAME, TEST_BIN and TEST_ARGS.
#
  IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TestSetup.cmake )
    INCLUDE( ${CMAKE_CURRENT_SOURCE_DIR}/TestSetup.cmake )
    IF( TEST_ARGS )
      FOREACH( ARGS ${TEST_ARGS} )
        IF( HOLD_NAME )
          SET( HOLD_ARGS ${ARGS} )
          SEPARATE_ARGUMENTS( HOLD_ARGS )
          ADD_TEST( ${TEST_NAME}-${HOLD_NAME} ${TEST_BIN} ${HOLD_ARGS} )
          SET( HOLD_NAME )
        ELSE( HOLD_NAME )
          SET( HOLD_NAME ${ARGS} )
        ENDIF( HOLD_NAME )
      ENDFOREACH( ARGS )
    ELSE( TEST_ARGS )
      ADD_TEST( ${TEST_NAME} ${TEST_BIN} )
    ENDIF( TEST_ARGS )
    ADD_EXECUTABLE( ${TEST_BIN} ${SOURCES} )
#
  ELSE( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TestSetup.cmake )
#
# In case no dedicated setup script has been supplied for the unit test, a
# generic name for the test is derived from the current source path and the
# test is registered under this designation with the CTest system...
#
    IF( DEFINED SOURCES )
      SET( TEST_BIN "hbem-003" )
      ADD_EXECUTABLE( ${TEST_BIN} ${SOURCES} )
    ENDIF( DEFINED SOURCES )
#
  ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/TestSetup.cmake )
#
  TARGET_LINK_LIBRARIES( ${TEST_BIN} CmdLine::CmdLine )
  TARGET_LINK_LIBRARIES( ${TEST_BIN} BEMLIB::BEMLIB )
  TARGET_LINK_LIBRARIES( ${TEST_BIN} ${UMFPACK_LIBRARY} )
#
###############################################################################
#
# If the target platform is MS Windows test drivers need to find & load any
# dynamically linked library the test driver has been linked against. To this
# end they are copied into CMakes's current binary directory:
#
  IF( WIN32 )
    ADD_CUSTOM_COMMAND( POST_BUILD
      TARGET ${TEST_BIN}
      COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:CmdLine::CmdLine>
        ${CMAKE_CURRENT_BINARY_DIR}
    )
  ENDIF( WIN32 )
#
###############################################################################
#
# If profiling information is required, the ENABLE_CODE_PROFILING variable
# must be defined. The profiling data can be obtained from the executable
# with the help of a tool called "gprof". Currently, this feature is not
# supported under the Windows operating system.
#
  IF( UNIX AND ENABLE_CODE_PROFILING )
    SET_TARGET_PROPERTIES( ${TEST_BIN} PROPERTIES LINK_FLAGS -pg )
  ENDIF( UNIX AND ENABLE_CODE_PROFILING )
#
# If information about test coverage is requested, the ENABLE_TEST_COVERAGE
# variable must be defined. The coverage data is generated on Unix systems
# with the help of the "gcov" tool. Currently, test coverage reports are not
# supported under the Windows operating system.
#
  IF( UNIX AND ENABLE_TEST_COVERAGE )
    TARGET_LINK_LIBRARIES( ${TEST_BIN} gcov )
  ENDIF( UNIX AND ENABLE_TEST_COVERAGE )
#
###############################################################################
