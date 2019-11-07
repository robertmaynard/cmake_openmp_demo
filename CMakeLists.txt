cmake_minimum_required(VERSION 3.15)
project(OpenMPIntelDemo
  HOMEPAGE_URL https://github.com/robertmaynard/cmake_openmp_demo
  DESCRIPTION "Showcase CMake+OpenMP with Intel compiler"
  VERSION 0.1.0
  LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(FetchContent)
include(CTest)

# State that C++14 is explicitly required to build our library
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()


# Build the lib that contains algorithms that use OpenMP
# We setup an alias of our library to what it would look like
# after installed
add_library(lib STATIC demo/lib.cxx)
add_library(demo::lib ALIAS lib)


#### Properties of our library

# Setup OpenMP
find_package(OpenMP 4.0 REQUIRED COMPONENTS CXX)
target_link_libraries(lib PUBLIC OpenMP::OpenMP_CXX)

# Setup our include directory
target_include_directories(lib PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
  )

# State to consumers that at least C++14 is required to use our library
target_compile_features(lib PUBLIC cxx_std_14)

# State that we need our library built with position independent code so
# that it can be consumed by a dynamic library (.so/.dylib/.dll)
set_target_properties(lib PROPERTIES POSITION_INDEPENDENT_CODE ON)


##### Test Logic
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CATCH_INSTALL_HELPERS OFF)
set(CATCH_BUILD_TESTING OFF)
set(CATCH_INSTALL_DOCS OFF)
FetchContent_Declare(Catch2 URL
    https://github.com/catchorg/Catch2/archive/v2.10.2.zip)
FetchContent_MakeAvailable(Catch2)

file(GLOB test-sources CONFIGURE_DEPENDS tests/*.cxx)
add_executable(${PROJECT_NAME}-tests "${test-sources}")
target_compile_definitions(${PROJECT_NAME}-tests
  PRIVATE
   CATCH_CONFIG_ENABLE_BENCHMARKING
  )
target_link_libraries(${PROJECT_NAME}-tests
  PRIVATE
    Catch2::Catch2
    demo::lib
  )

include(${Catch2_SOURCE_DIR}/contrib/Catch.cmake)
catch_discover_tests(${PROJECT_NAME}-tests)

###### Install Logic
install(TARGETS lib
  EXPORT ${PROJECT_NAME}-targets
  INCLUDES DESTINATION "${CMAKE_INSTALL_DATADIR}")

install(EXPORT ${PROJECT_NAME}-targets
  DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
  NAMESPACE demo::
  FILE "${PROJECT_NAME}-targets.cmake")
