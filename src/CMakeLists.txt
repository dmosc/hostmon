add_library(
    hostmon_core
    STATIC
    hostmon/core/periodic_runner.cc
)
add_library(hostmon::core ALIAS hostmon_core)

target_include_directories(hostmon_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hostmon_core PUBLIC cxx_std_23)
hostmon_enable_warnings(hostmon_core)
