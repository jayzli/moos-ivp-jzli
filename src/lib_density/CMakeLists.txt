#--------------------------------------------------------
# The CMakeLists.txt for:              lib_density
# Author(s):                                            John Li
#--------------------------------------------------------

SET(SRC
 DensityCounter.cpp
  )

SET(HEADERS
  DensityCounter.h
)

# Build Library
ADD_LIBRARY(density ${SRC})

TARGET_LINK_LIBRARIES( density
   mbutil
   apputil
)

