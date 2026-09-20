add_library(hostmon_core INTERFACE)
add_library(hostmon::core ALIAS hostmon_core)

target_include_directories(hostmon_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hostmon_core INTERFACE cxx_std_23)
