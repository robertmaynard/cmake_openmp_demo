# Based on the work from https://github.com/TartanLlama/expected
# Thanks to Isabella Muerte
cmake_minimum_required(VERSION 3.15)
project(OpenMPIntelDemo
  HOMEPAGE_URL https://github.com/robertmaynard/cmake_openmp_demo
  DESCRIPTION "Showcase CMake+OpenMP with Intel compiler"
  VERSION 0.1.0
  LANGUAGES CXX
  )

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FetchContent)
include(CTest)

### C++ Standard Level

# State that C++14 is explicitly required to build our library
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()


### Add Library

# Build the lib that contains algorithms that use OpenMP
# We setup an alias of our library to what it would look like
# after installed
add_library(lib STATIC demo/lib.cxx)
add_library(demo::lib ALIAS lib)


#### Properties of our library

# Setup OpenMP
find_package(OpenMP 4.0 REQUIRED COMPONENTS CXX)
target_link_libraries(lib PRIVATE OpenMP::OpenMP_CXX)

# Setup our include directory
# The INSTALL_INTERFACE is what is evaluated when consumers build
# linking to this target. Relative paths are allowed, and are evaluated
# as being relative to the install location.
# Otherwise the BUILD_INTERFACE is evaluated, and those always need
# to be absolute paths
target_include_directories(lib PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:"include/">
  )

# State to consumers that at least C++14 is required to use our library
target_compile_features(lib PUBLIC cxx_std_14)

# State that we need our library built with position independent code so
# that it can be consumed by a dynamic library (.so/.dylib/.dll)
set_target_properties(lib PROPERTIES POSITION_INDEPENDENT_CODE ON)


### Add Tests

# Setup Catch2 for Tests
if(BUILD_TESTING)
  # Use FetchContent to to download and make Catch2 part of our CMake build.
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
  set(CATCH_INSTALL_HELPERS OFF)
  set(CATCH_BUILD_TESTING OFF)
  set(CATCH_INSTALL_DOCS OFF)
  FetchContent_Declare(Catch2 URL
      https://github.com/catchorg/Catch2/archive/v2.10.2.zip)
  FetchContent_MakeAvailable(Catch2)

  # Use a glob that re-runs everytime we build to get all the
  # tests sources
  file(GLOB test-sources CONFIGURE_DEPENDS tests/*.cxx)
  add_executable(${PROJECT_NAME}-tests "${test-sources}")

  # Mark the tests as needing Catch benchmark infrastructure which
  # off by default
  target_compile_definitions(${PROJECT_NAME}-tests
    PRIVATE
    CATCH_CONFIG_ENABLE_BENCHMARKING
    )

  #
  target_link_libraries(${PROJECT_NAME}-tests
    PRIVATE
      Catch2::Catch2
      demo::lib
    )

  # catch_discover_tests will automatically convert each catch2
  # test case into a separate ctest entry
  include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)
  catch_discover_tests(${PROJECT_NAME}-tests)
endif()

### Setup the project to be installable / exportable / importable

# CMake has the concept of an importable project. This means a project
# that can be found using `find_package` without a custom FindProject being
# written by the consumer. Instead the project to be found, exports itself.
#
# Exporting is primarily done by generating two files:
#   <project_name>-targets.cmake which contains all the targets consumers can
#   use
#   <project_name>-config.cmake which is the primary entry point for cmake
#   and contains any required `find_package` calls to use your project and
#   includes <project_name>-targets.cmake.

# Install our headers
install(FILES
  ${PROJECT_SOURCE_DIR}/demo/lib.h
  DESTINATION "include/demo"
  )

# Configure the config.cmake file
# processes the <project_name>-config.cmake template and place it in the build
# directory so it doesn't pollute our source directory
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
  "${PROJECT_BINARY_DIR}/share/cmake/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION "share/cmake/${PROJECT_NAME}"
  )

# Add an install rule for the static library we built
# `EXPORT` states that lib is part of the set of targets we want exported
# to <project_name>-targets.cmake.
install(TARGETS lib EXPORT ${PROJECT_NAME}-targets)

# Install all targets that are part of the export set in the
# <project_name>-targets.cmake under the namespace 'demo'.
install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NAMESPACE demo::
  FILE "${PROJECT_NAME}-targets.cmake"
  )

# Install the <project_name>-config.cmake file that we configured into
# the build directory
install(FILES
  "${PROJECT_BINARY_DIR}/share/cmake/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  )
