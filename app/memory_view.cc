#include "memory_view.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <ftxui/dom/elements.hpp>
#include <vector>

#include "hostmon/memory/memory_types.h"

namespace hostmon
{
namespace
{
ftxui::Color LoadColor(double fraction)
{
  if (fraction > 0.90) return ftxui::Color::Red;
  if (fraction > 0.70) return ftxui::Color::Yellow;
  return ftxui::Color::Green;
}

ftxui::Element UsageRow(const std::string& label, std::uint64_t bytes, double fraction)
{
  return ftxui::hbox({
      ftxui::text(label) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 12),
      ftxui::gauge(static_cast<float>(fraction)) | ftxui::color(LoadColor(fraction)) | ftxui::flex,
      ftxui::text(std::format(" {}", bytes)) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 10),
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

ftxui::Component MakeMemoryView(const MemoryMonitor& monitor)
{
  return ftxui::Renderer(
      [&monitor]() -> ftxui::Element
      {
        const auto latest = monitor.Latest();
        if (!latest)
        {
          return ftxui::window(ftxui::text(" Memory "), ftxui::text("Collecting first sample..."));
        }
        ftxui::Elements rows;
        rows.push_back(UsageRow("Used", latest->used_bytes(), latest->used_fraction()));
        rows.push_back(HistoryGraph(monitor.UsedHistory()) | ftxui::color(ftxui::Color::Magenta) |
                       ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 6));
        rows.push_back(ftxui::separator());
        rows.push_back(UsageRow(
            "Wired", latest->wired_bytes,
            static_cast<double>(latest->wired_bytes) / static_cast<double>(latest->total_bytes)));
        rows.push_back(UsageRow("Compressed", latest->compressed_bytes,
                                static_cast<double>(latest->compressed_bytes) /
                                    static_cast<double>(latest->total_bytes)));
        rows.push_back(UsageRow(
            "Free", latest->free_bytes,
            static_cast<double>(latest->free_bytes) / static_cast<double>(latest->total_bytes)));
        return ftxui::window(ftxui::text(" Memory ") | ftxui::bold, ftxui::vbox(std::move(rows))) |
               ftxui::flex;
      });
}
}  // namespace hostmon