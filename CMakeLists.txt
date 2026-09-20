cmake_minimum_required(VERSION 3.25)
project(hostmon VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(HOSTMON_SANITIZE "Build with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(HOSTMON_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

include(cmake/CompilerWarnings.cmake)

if(HOSTMON_SANITIZE)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

find_package(ftxui CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(app)

include(CTest)
if(BUILD_TESTING)
  find_package(GTest CONFIG REQUIRED)
  add_subdirectory(tests)
endif()
