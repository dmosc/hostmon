#include "cpu_view.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

#include "hostmon/cpu/cpu_types.h"

namespace hostmon
{
namespace
{
ftxui::Color LoadColor(double busy)
{
  if (busy > 0.85) return ftxui::Color::Red;
  if (busy > 0.60) return ftxui::Color::Yellow;
  return ftxui::Color::Green;
}

ftxui::Element UsageRow(const std::string& label, const CpuUsage& usage)
{
  using namespace ftxui;
  return hbox({
      text(label) | size(WIDTH, EQUAL, 6),
      gauge(static_cast<float>(usage.busy())) | color(LoadColor(usage.busy())) | flex,
      text(std::format(" {:5.1f}%", usage.busy() * 100.0)) | size(WIDTH, EQUAL, 8),
  });
}

ftxui::Element HistoryGraph(std::vector<double> history)
{
  auto column_heights = [history = std::move(history)](int width, int height)
  {
    const auto columns = static_cast<std::size_t>(std::max(width, 0));
    std::vector<int> heights(columns, 0);
    const std::size_t count = std::min(columns, history.size());
    for (std::size_t idx = 0; idx < count; idx++)
    {
      const double sample = history[history.size() - count + idx];
      heights[columns - count + idx] = static_cast<int>(std::lround(sample * height));
    }
    return heights;
  };
  return ftxui::graph(std::move(column_heights));
}
}  // namespace

ftxui::Component MakeCpuView(const CpuMonitor& monitor)
{
  return ftxui::Renderer(
      [&monitor]() -> ftxui::Element
      {
        const auto monitor_latest = monitor.Latest();
        if (!monitor_latest)
        {
          return ftxui::window(ftxui::text("CPU"), ftxui::text("Collecting first sample..."));
        }
        ftxui::Elements elements;
        elements.push_back(UsageRow("All", monitor_latest->aggregate));
        elements.push_back(HistoryGraph(monitor.BusyHistory()) | ftxui::color(ftxui::Color::Cyan) |
                           ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 8));
        elements.push_back(ftxui::separator());
        for (std::size_t idx = 0; idx < monitor_latest->cores.size(); idx++)
        {
          elements.push_back(UsageRow(std::format("CPU{}", idx), monitor_latest->cores[idx]));
        }
        return ftxui::window(ftxui::text(" CPU ") | ftxui::bold,
                             ftxui::vbox(std::move(elements)) | ftxui::flex);
      });
}
}  // namespace hostmon
