add_executable(hostmon main.cc cpu_view.cc)
target_link_libraries(hostmon PRIVATE
  hostmon::core
  ftxui::screen ftxui::dom ftxui::component)
hostmon_enable_warnings(hostmon)
