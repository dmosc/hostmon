add_library(
  hostmon_core
  STATIC
  hostmon/core/periodic_runner.cc
  hostmon/cpu/cpu_monitor.cc
  hostmon/cpu/cpu_usage.cc
  hostmon/memory/memory_monitor.cc
  hostmon/memory/memory_usage.cc
)
add_library(hostmon::core ALIAS hostmon_core)

if(APPLE)
  target_sources(hostmon_core PRIVATE
    hostmon/platform/macos/mach_cpu_tick_source.cc
    hostmon/platform/macos/mach_memory_sample_source.cc
  )
endif()

target_include_directories(hostmon_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hostmon_core PUBLIC cxx_std_23)
hostmon_enable_warnings(hostmon_core)
