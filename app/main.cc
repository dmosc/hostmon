#include <chrono>
#include <ftxui/component/app.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>

#include "cpu_view.h"
#include "hostmon/core/periodic_runner.h"
#include "hostmon/cpu/cpu_monitor.h"
#include "hostmon/memory/memory_monitor.h"
#include "hostmon/platform/macos/mach_cpu_tick_source.h"
#include "hostmon/platform/macos/mach_memory_sample_source.h"
#include "memory_view.h"

int main()
{
  using namespace std::chrono_literals;
  hostmon::CpuMonitor cpu_monitor(std::make_unique<hostmon::MachCpuTickSource>(), 240);
  hostmon::MemoryMonitor memory_monitor(std::make_unique<hostmon::MachMemorySampleSource>(), 240);
  auto app = ftxui::App::Fullscreen();
  hostmon::PeriodicRunner sampler(1s,
                                  [&]
                                  {
                                    // Surface sampling error in the UI.
                                    (void)cpu_monitor.Poll();
                                    (void)memory_monitor.Poll();
                                    app.PostEvent(ftxui::Event::Custom);
                                  });
  auto cpu_view = hostmon::MakeCpuView(cpu_monitor);
  auto memory_view = hostmon::MakeMemoryView(memory_monitor);
  auto dashboard = ftxui::Renderer(
      [cpu_view, memory_view]
      {
        return ftxui::vbox({
            cpu_view->Render(),
            memory_view->Render(),
        });
      });
  auto root = ftxui::CatchEvent(dashboard,
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