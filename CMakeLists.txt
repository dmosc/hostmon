cmake_minimum_required(VERSION 3.20)
project(hostmon CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(ftxui CONFIG REQUIRED)
add_executable(hostmon main.cc)
target_link_libraries(
    hostmon PRIVATE
    ftxui::screen
    ftxui::dom
)