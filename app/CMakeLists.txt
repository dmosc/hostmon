add_executable(hostmon main.cc)
target_link_libraries(hostmon PRIVATE
  hostmon::core
  ftxui::screen ftxui::dom ftxui::component)
hostmon_enable_warnings(hostmon)
