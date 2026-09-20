add_executable(hostmon_tests
  cpu_monitor_test.cc
  cpu_usage_test.cc
  ring_buffer_test.cc
)
target_link_libraries(hostmon_tests PRIVATE hostmon::core GTest::gmock_main)
hostmon_enable_warnings(hostmon_tests)

include(GoogleTest)
gtest_discover_tests(hostmon_tests)
