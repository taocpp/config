cmake_minimum_required(VERSION 3.8...3.19)

set(examplesources
  dump_all_phases.cpp
  dump_only_data.cpp
  dump_phase_four.cpp
  dump_phase_one.cpp
  dump_phase_three.cpp
  dump_phase_two.cpp
  dump_with_meta.cpp
)

# file(GLOB ...) is used to validate the above list of example_sources
file(GLOB glob_example_sources RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp)

foreach(examplesourcefile ${examplesources})
  if(${examplesourcefile} IN_LIST glob_example_sources)
    list(REMOVE_ITEM glob_example_sources ${examplesourcefile})
  else()
    message(SEND_ERROR "File ${examplesourcefile} is missing from src/example/config")
  endif()
  get_filename_component(exename ${examplesourcefile} NAME_WE)
  set(exename "tao-config-example-${exename}")
  add_executable(${exename} ${examplesourcefile})
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
endforeach()

if(glob_example_sources)
  foreach(ignored_source_file ${glob_example_sources})
    message(SEND_ERROR "File ${ignored_source_file} in src/example/config is ignored")
  endforeach()
endif()
