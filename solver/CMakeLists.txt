
# Specify the name of the executable to be built by the project:

SET( SOLVER "hbem" )

# Specifiy the name of the object library used to compile & link
# the various test drivers:

SET( LIBNAME "BEM2D" )

# Define the folders containing the source code of the solver.

ADD_SUBDIRECTORY( "src" )
ADD_SUBDIRECTORY( "tests" )
