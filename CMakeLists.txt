cmake_minimum_required(VERSION 2.8)
project( life )
find_package( OpenCV REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLUT REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
add_executable( life life.cpp )
target_link_libraries( life ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} )