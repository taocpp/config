cmake_minimum_required(VERSION 3.8...3.19)

set(testsources
  access.cpp
  assign.cpp
  custom.cpp
  enumerations.cpp
  failure.cpp
  parse_key1.cpp
  parse_key.cpp
  parse_reference2.cpp
  success.cpp
)

# file(GLOB ...) is used to validate the above list of test_sources
file(GLOB glob_test_sources RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

foreach(testsourcefile ${testsources})
  if(${testsourcefile} IN_LIST glob_test_sources)
    list(REMOVE_ITEM glob_test_sources ${testsourcefile})
  else()
    message(SEND_ERROR "File ${testsourcefile} is missing from src/test/config")
  endif()
  get_filename_component(exename ${testsourcefile} NAME_WE)
  set(exename "tao-config-test-${exename}")
  add_executable(${exename} ${testsourcefile})
  target_link_libraries(${exename} PRIVATE taocpp::config)
  set_target_properties(${exename} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  if(MSVC)
    target_compile_options(${exename} PRIVATE /W4 /WX /utf-8 /bigobj)
  else()
    target_compile_options(${exename} PRIVATE -pedantic -Wall -Wextra -Werror)
  endif()
  add_test(NAME ${exename} WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../.. COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${exename})
endforeach()

if(glob_test_sources)
  foreach(ignored_source_file ${glob_test_sources})
    message(SEND_ERROR "File ${ignored_source_file} in src/test/config is ignored")
  endforeach()
endif()
