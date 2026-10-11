#pragma once

#include <ftxui/component/component.hpp>

#include "hostmon/memory/memory_monitor.h"

namespace hostmon
{
// Renders memory usage from `monitor`.
[[nodiscard]] ftxui::Component MakeMemoryView(const MemoryMonitor& monitor);
}  // namespace hostmon