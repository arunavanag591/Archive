cmake_minimum_required(VERSION 2.8.3)
project(auto_nav)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  message_generation
)

###################################
## catkin specific configuration ##
###################################

catkin_package(
   INCLUDE_DIRS include
   DEPENDS system_lib
   LIBRARIES auto_nav
   CATKIN_DEPENDS roscpp std_msgs 
)

################################################
## Declare ROS messages, services and actions ##
################################################


###########
## Build ##
###########

include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS} 
)

add_library(sensor_lib
  src/sensor_description.cpp
)

add_dependencies(sensor_lib ${catkin_EXPORTED_TARGETS} )

add_executable(sensor_node src/sensor_node.cpp)
add_dependencies(sensor_node ${catkin_EXPORTED_TARGETS} sensor_lib)
target_link_libraries(sensor_node ${catkin_LIBRARIES} sensor_lib)

#############
## Install ##
#############

install(TARGETS sensor_lib sensor_node
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(DIRECTORY include/${PROJECT_NAME}/
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)

install(DIRECTORY launch/
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
  PATTERN ".svn" EXCLUDE)

#############
## Testing ##
#############

if(CATKIN_ENABLE_TESTING)
    find_package(rostest REQUIRED)
    
 endif()
