cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ivutils CXX)
set(CMAKE_CXX_STANDARD 14)
set(PROJECT_VERSION 1)
set(IVUTILS_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR})

#----- find dependencies
find_package(PythonLibs 2.7 REQUIRED)
message(STATUS "Python v${PYTHONLIBS_VERSION_STRING} found")
include_directories(${PYTHON_INCLUDE_DIRS})
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
set(GPIB_LIBRARY "")
if(EMULATE)
  message(STATUS "GPIB emulation mode enabled")
  add_definitions(-DEMULATE)
else()
  find_library(LINUX_GPIB gpib)
  find_library(NI4882 ni4882 HINTS /usr/lib/x86_64-linux-gnu)
  if(LINUX_GPIB)
    message(STATUS "LinuxGPIB library found")
    add_definitions(-DGPIB)
    set(GPIB_LIBRARY ${LINUX_GPIB})
  elseif(NI4882)
    message(STATUS "NI488.2 library found")
    add_definitions(-DNI4882)
    set(GPIB_LIBRARY ${NI4882})
  endif()
endif()

#----- define the library

file(GLOB IVUTILS_SOURCES ${IVUTILS_SOURCE_DIR}/*.cc)
add_library(ivutils SHARED ${IVUTILS_SOURCES})
target_link_libraries(ivutils ${PYTHON_LIBRARIES} ${ROOT_LIBRARIES})

#----- copy the input cards and other files

file(GLOB_RECURSE input_cards RELATIVE ${PROJECT_SOURCE_DIR} cards/*)
foreach(_files ${input_cards})
  configure_file(${_files} ${_files} COPYONLY)
endforeach()

#----- installation rules

install(DIRECTORY ivutils DESTINATION include)

#----- set the tests/utils directory

file(GLOB tests RELATIVE ${PROJECT_SOURCE_DIR}/test ${PROJECT_SOURCE_DIR}/test/*.cc)
foreach(_test ${tests})
  string(REPLACE ".cc" "" test_bin ${_test})
  add_executable(${test_bin} ${PROJECT_SOURCE_DIR}/test/${_test})
  set_target_properties(${test_bin} PROPERTIES EXCLUDE_FROM_ALL true)
  target_link_libraries(${test_bin} ivutils ${GPIB_LIBRARY})
endforeach()

#----- documentation

find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
  set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
  message(STATUS "Doxygen build started")
  add_custom_target(doc_doxygen COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation with Doxygen" VERBATIM)
endif()

