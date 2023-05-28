cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ice VERSION 0.1.0 LANGUAGES CXX)

# ice::ice
add_library(ice INTERFACE)
add_library(ice::ice ALIAS ice)

# ice::example
add_library(example)
set_target_properties(example PROPERTIES OUTPUT_NAME ice-example)
target_compile_features(example PUBLIC cxx_std_23)

find_package(ACE REQUIRED)
target_link_libraries(example PUBLIC ACE::STD)

target_sources(example PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
  ice/example/format.cpp
  ice/example/modify.cpp
  ice/example.cpp)

target_compile_options(example PUBLIC
  "$<BUILD_INTERFACE:-fprebuilt-module-path=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/example.dir/$<CONFIG>>")

add_library(ice::example ALIAS example)
target_link_libraries(ice INTERFACE example)

# install
if(PROJECT_IS_TOP_LEVEL)
  install(TARGETS ice example EXPORT ice
    RUNTIME DESTINATION "$<CONFIG>/bin"
    LIBRARY DESTINATION "$<CONFIG>/lib"
    ARCHIVE DESTINATION "$<CONFIG>/lib"
    FILE_SET cxx_modules DESTINATION "$<CONFIG>"
    CXX_MODULES_BMI DESTINATION "$<CONFIG>")
endif()

# executable
if(PROJECT_IS_TOP_LEVEL)
  add_executable(main main.cpp main.manifest main.rc)
  target_link_libraries(main PRIVATE ice::ice)
endif()
