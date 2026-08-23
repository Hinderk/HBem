
###############################################################################
#
# Define the source files which must be compiled into the test driver...
#
  SET( SOURCES "main.cpp" )
#
###############################################################################
#
  SET( TEST_BIN "plugin-001" )
  SET( TESTNAME "dirichlet-01" )

  ADD_EXECUTABLE( ${TEST_BIN} ${SOURCES} )
  TARGET_LINK_LIBRARIES( ${TEST_BIN} BEMLIB::BEMLIB )
  TARGET_LINK_LIBRARIES( ${TEST_BIN} ${UMFPACK_LIBRARY} )
#
  ADD_TEST( ${TESTNAME} ${TEST_BIN} )
#
###############################################################################
