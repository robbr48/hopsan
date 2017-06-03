cmake_minimum_required(VERSION 3.0)
project(HopsanCore)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_DEBUG_POSTFIX _d)


file(GLOB_RECURSE HopsanCore_srcfiles src/*.cc)
file(GLOB_RECURSE HopsanCore_incfiles include/*.h include/*.hpp)

file(GLOB_RECURSE numhop_srcfiles dependencies/libNumHop/libNumHop/*.cc)
file(GLOB_RECURSE csvparser_srcfiles dependencies/IndexingCSVParser/IndexingCSVParser.cpp)

add_library(HopsanCore SHARED ${HopsanCore_srcfiles}
			      ${numhop_srcfiles}
			      ${csvparser_srcfiles})
target_include_directories(HopsanCore PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> 
  $<INSTALL_INTERFACE:include>)

target_include_directories(HopsanCore PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libNumHop/libNumHop>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/IndexingCSVParser>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/dependencies/rapidxml>) 

target_link_libraries(HopsanCore ${CMAKE_DL_LIBS})

install(TARGETS HopsanCore
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION include )