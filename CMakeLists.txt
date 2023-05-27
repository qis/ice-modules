cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ice VERSION 0.1.0 LANGUAGES CXX)

# Options
option(ICE_BUILD_MAIN "Build main executable." ${PROJECT_IS_TOP_LEVEL})

# Standard C++ Modules
find_package(std REQUIRED)

# ice::example
add_library(example main.rc)
target_compile_features(example PUBLIC cxx_std_23)
target_link_libraries(example PUBLIC std::std)

target_sources(example PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
  ice/example/format.cpp
  ice/example/modify.cpp
  ice/example.cpp)

add_library(ice::example ALIAS example)

# ice::ice
add_library(ice main.rc)
target_compile_features(ice PUBLIC cxx_std_23)
target_link_libraries(ice PUBLIC example)

target_sources(ice PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
  ice/ice.cpp)

add_library(ice::ice ALIAS ice)

# install
install(TARGETS example ice
  ARCHIVE DESTINATION "$<CONFIG>"
  CXX_MODULES_BMI DESTINATION "$<CONFIG>")

# executable
if(ICE_BUILD_MAIN)
  add_executable(main main.cpp main.manifest main.rc)
  target_link_libraries(main PRIVATE ice::ice)
endif()
