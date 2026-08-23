
CMAKE_MINIMUM_REQUIRED( VERSION 3.15.0 )

PROJECT( HBem C CXX )

# Locate the threading libraries to link the HBem solver against ...

FIND_PACKAGE( "Threads" )

# Locate any third party libraries required to build the solver:

FIND_PACKAGE( "Umfpack" )

# The following switch turns the automated unit tests on or off. To
# disable the compilation of test code simply remove the line below.

ENABLE_TESTING()

# Add contributions by third parties used by the HBem solver here:

ADD_SUBDIRECTORY( "libs" )

# Define the folders containing the source code of the HBem solver:

ADD_SUBDIRECTORY( "solver" )

# The folder below contains shared objects that implement a mechanism
# to enforce arbitrary boundary conditions:

ADD_SUBDIRECTORY( "plugins" )
