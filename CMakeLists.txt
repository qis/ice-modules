cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(ice VERSION 0.1.0 LANGUAGES CXX)

add_library(ice main.rc)
add_library(ice::ice ALIAS ice)
target_compile_features(ice PUBLIC cxx_std_23)

file(GLOB_RECURSE sources src/*.cpp)
target_sources(ice PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES ${sources})

target_compile_options(ice PUBLIC
  "$<BUILD_INTERFACE:-fprebuilt-module-path=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ice.dir/$<CONFIG>>")

#find_package(ACE REQUIRED)
#target_link_libraries(ice PUBLIC ACE::STD)

if(PROJECT_IS_TOP_LEVEL)
  install(TARGETS ice
    RUNTIME DESTINATION "$<CONFIG>/bin"
    LIBRARY DESTINATION "$<CONFIG>/lib"
    ARCHIVE DESTINATION "$<CONFIG>/lib"
    CXX_MODULES_BMI DESTINATION "$<CONFIG>")
endif()

if(PROJECT_IS_TOP_LEVEL)
  add_executable(main main.cpp main.manifest main.rc)
  target_link_libraries(main PRIVATE ice::ice)

  find_package(Boost REQUIRED COMPONENTS asio beast)
  target_link_libraries(main PRIVATE Boost::asio Boost::beast)

  find_package(Threads REQUIRED)
  target_link_libraries(main PRIVATE Threads::Threads)
endif()
