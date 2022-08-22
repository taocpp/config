cmake_minimum_required(VERSION 3.8...3.19)

project(taocpp-config VERSION 1.0.0 LANGUAGES CXX)

set(TAOCPP_CONFIG_IS_MAIN_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(TAOCPP_CONFIG_IS_MAIN_PROJECT ON)
endif()

# installation directories
include(GNUInstallDirs)

set(TAOCPP_CONFIG_INSTALL_DOC_DIR "${CMAKE_INSTALL_DOCDIR}/tao/config" CACHE STRING "The installation doc directory")
set(TAOCPP_CONFIG_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake" CACHE STRING "The installation cmake directory")

# define a header-only library
add_library(taocpp-config INTERFACE)
add_library(taocpp::config ALIAS taocpp-config)
target_include_directories(taocpp-config INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# require C++17
target_compile_features(taocpp-config INTERFACE cxx_std_17)

# find a suitable taoJSON
set(TAOCPP_CONFIG_JSON_MIN_VERSION 1.0.0)
find_package(taocpp-json ${TAOCPP_CONFIG_JSON_MIN_VERSION} QUIET CONFIG)
if(NOT taocpp-json_FOUND)
  # if a compatible version of taoJSON is not already installed, build and install it from the submodule directory
  message(STATUS "Adding taoJSON as submodule from external/json")
  set(TAOCPP_JSON_INSTALL ON)
  set(TAOCPP_JSON_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE STRING "Override taoJSON include install directory")
  set(TAOCPP_JSON_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_DATAROOTDIR}/json/cmake CACHE STRING "Override taoJSON cmake install directory")
  add_subdirectory(external/json)
endif()

# add taoJSON as a dependency
target_link_libraries(taocpp-config INTERFACE taocpp-json)

# testing
option(TAOCPP_CONFIG_BUILD_TESTS "Build test programs" ${TAOCPP_CONFIG_IS_MAIN_PROJECT})
if(TAOCPP_CONFIG_BUILD_TESTS)
  enable_testing()
  add_subdirectory(src/test/config)
endif()

# examples
option(TAOCPP_CONFIG_BUILD_EXAMPLES "Build example programs" ${TAOCPP_CONFIG_IS_MAIN_PROJECT})
if(TAOCPP_CONFIG_BUILD_EXAMPLES)
  add_subdirectory(src/example/config)
endif()

option(TAOCPP_CONFIG_INSTALL "Generate the install target" ${TAOCPP_CONFIG_IS_MAIN_PROJECT})
if(TAOCPP_CONFIG_INSTALL)
  include(CMakePackageConfigHelpers)

  # Make package findable
  configure_package_config_file(.cmake/taocpp-config-config.cmake.in ${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${TAOCPP_CONFIG_INSTALL_CMAKE_DIR}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
    NO_SET_AND_CHECK_MACRO
  )

  # Ignore pointer width differences since this is a header-only library
  unset(CMAKE_SIZEOF_VOID_P)

  # Enable version checks in find_package
  write_basic_package_version_file(${PROJECT_NAME}-config-version.cmake COMPATIBILITY SameMajorVersion)

  # keep taocpp::config target compatibility
  set_target_properties(taocpp-config PROPERTIES EXPORT_NAME config)

  # install and export target
  install(TARGETS taocpp-config taocpp-json EXPORT ${PROJECT_NAME}-targets)
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${TAOCPP_CONFIG_INSTALL_CMAKE_DIR}
  )
  install(EXPORT ${PROJECT_NAME}-targets
    NAMESPACE taocpp::
    DESTINATION ${TAOCPP_CONFIG_INSTALL_CMAKE_DIR}
  )

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
  install(FILES LICENSE DESTINATION ${TAOCPP_CONFIG_INSTALL_DOC_DIR})
endif()
