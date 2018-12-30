cmake_minimum_required(VERSION 2.8.3)
project(tcp_comms)

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
  rospy
  std_msgs
  message_generation
)


catkin_package(
   INCLUDE_DIRS include
   LIBRARIES tcp_comms
   CATKIN_DEPENDS roscpp rospy std_msgs 
   DEPENDS system_lib
)


include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_library(tcp_comms_lib
  src/tcp_comms.cpp
)
add_library(tcp_comms_test_lib
  src/tcp_test.cpp
)


add_dependencies(tcp_comms_lib ${catkin_EXPORTED_TARGETS} )
add_dependencies(tcp_comms_test_lib ${catkin_EXPORTED_TARGETS} )

add_executable(tcp_comms src/tcp_comms_node.cpp)
add_dependencies(tcp_comms ${catkin_EXPORTED_TARGETS} tcp_comms_lib)
target_link_libraries(tcp_comms ${catkin_LIBRARIES} tcp_comms_lib)

add_executable(tcp_comms_test src/tcp_comms_test_node.cpp)
add_dependencies(tcp_comms_test ${catkin_EXPORTED_TARGETS} tcp_comms_test_lib)
target_link_libraries(tcp_comms_test ${catkin_LIBRARIES} tcp_comms_test_lib)


install(TARGETS  tcp_comms_test_lib tcp_comms_test tcp_comms tcp_comms_lib
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


if(CATKIN_ENABLE_TESTING)
    find_package(rostest REQUIRED)

endif()
