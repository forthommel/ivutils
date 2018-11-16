cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ivutils CXX)
set(CMAKE_CXX_STANDARD 11)
set(PROJECT_VERSION 1)
set(IVUTILS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR})

#----- find dependencies
find_package(PythonLibs 2.7 REQUIRED)
message(STATUS "Python v${PYTHONLIBS_VERSION_STRING} found")
include_directories(${PYTHON_INCLUDE_DIRS})

#----- define the library

file(GLOB IVUTILS_SOURCES ${IVUTILS_SOURCE_DIR}/*.cc)
add_library(ivutils SHARED ${IVUTILS_SOURCES})
target_link_libraries(ivutils ${PYTHON_LIBRARIES})

#----- copy the input cards and other files

file(GLOB_RECURSE input_cards RELATIVE ${PROJECT_SOURCE_DIR} cards/*)
foreach(_files ${input_cards})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

#----- installation rules

install(DIRECTORY ivutils DESTINATION include)

#----- set the tests/utils directory

#add_subdirectory(test)

#----- documentation

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message(STATUS "Doxygen build started")
  add_custom_target(doc_doxygen COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation with Doxygen" VERBATIM)
endif()

