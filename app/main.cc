#include <chrono>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <memory>

#include "cpu_view.h"
#include "hostmon/core/periodic_runner.h"
#include "hostmon/cpu/cpu_monitor.h"
#include "hostmon/platform/macos/mach_cpu_tick_source.h"

int main()
{
  using namespace std::chrono_literals;
  hostmon::CpuMonitor cpu_monitor(std::make_unique<hostmon::MachCpuTickSource>(), 240);
  auto app = ftxui::App::Fullscreen();
  hostmon::PeriodicRunner sampler(1s,
                                  [&]
                                  {
                                    if (auto result = cpu_monitor.Poll(); !result)
                                    {
                                      // Surface sampling error in the UI.
                                    }
                                    app.PostEvent(ftxui::Event::Custom);
                                  });
  auto root = ftxui::CatchEvent(hostmon::MakeCpuView(cpu_monitor),
                                [&](const ftxui::Event& event)
                                {
                                  if (event == ftxui::Event::Character('q'))
                                  {
                                    app.Exit();
                                    return true;
                                  }
                                  return false;
                                });
  app.Loop(root);
  return 0;
}