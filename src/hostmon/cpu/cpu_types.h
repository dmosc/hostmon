#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include "hostmon/core/clock.h"

namespace hostmon
{
// Cumulative time one core has spent in each state since boot. We rely on
// ratios since the unit of measurement provided by the OS is opaque ("ticks").
struct CpuTicks
{
  std::uint32_t user = 0;
  std::uint32_t system = 0;
  std::uint32_t nice = 0;
  std::uint32_t idle = 0;

  friend bool operator==(const CpuTicks&, const CpuTicks&) = default;
};

struct CpuTickSnapshot
{
  Clock::time_point taken_at;
  std::vector<CpuTicks> cores;
};

// Fractions of a [0, 1] interval summing to 1.
struct CpuUsage
{
  double user = 0.0;
  double system = 0.0;
  double nice = 0.0;
  double idle = 1.0;

  [[nodiscard]] double busy() const noexcept { return user + system + nice; }
};

struct CpuUsageSnapshot
{
  Clock::time_point taken_at;
  std::chrono::nanoseconds interval{};
  CpuUsage aggregate;
  std::vector<CpuUsage> cores;
};
}  // namespace hostmon
