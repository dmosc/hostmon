#pragma once

#include <ftxui/component/component.hpp>

#include "hostmon/cpu/cpu_monitor.h"

namespace hostmon
{
// Renders CPU usage from `monitor`.
[[nodiscard]] ftxui::Component MakeCpuView(const CpuMonitor& monitor);
}  // namespace hostmon