#--------------------------------------------------------
# The CMakeLists.txt for:                       pTrafficDensity
# Author(s):                              John Li
#--------------------------------------------------------

SET(SRC
  TrafficDensity.cpp
  TrafficDensity_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pTrafficDensity ${SRC})

TARGET_LINK_LIBRARIES(pTrafficDensity
   ${MOOS_LIBRARIES}
   ${MOOSGeodesy_LIBRARIES}
   mbutil
   apputil
   pthread
   contacts
   density
   geometry
)

