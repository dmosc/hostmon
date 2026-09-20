file(GLOB_RECURSE HOSTMON_TEST_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*_test.cc)

add_executable(hostmon_tests ${HOSTMON_TEST_SOURCES})
target_link_libraries(hostmon_tests PRIVATE hostmon::core GTest::gmock_main)
hostmon_enable_warnings(hostmon_tests)

include(GoogleTest)
gtest_discover_tests(hostmon_tests)
