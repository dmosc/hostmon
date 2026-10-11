#pragma once

#include <cstdint>

#include "hostmon/core/clock.h"

namespace hostmon
{
// Raw page counts from the kernel's VM statistics, in units of the
// machine's page size.
struct VmPageCounts
{
  std::uint64_t free = 0;
  std::uint64_t active = 0;
  std::uint64_t inactive = 0;
  std::uint64_t wired = 0;
  std::uint64_t compressed = 0;
  std::uint64_t speculative = 0;
};

struct MemorySample
{
  Clock::time_point taken_at;
  std::uint64_t page_size = 0;
  VmPageCounts pages;
  // From sysctl hw.memsize; fixed at boot.
  std::uint64_t physical_total_bytes = 0;
};

// Byte-denominated view derived from a MemorySample. Buckets are mutually
// exclusive.
struct MemoryUsage
{
  std::uint64_t total_bytes = 0;
  std::uint64_t free_bytes = 0;
  std::uint64_t active_bytes = 0;
  std::uint64_t inactive_bytes = 0;
  std::uint64_t wired_bytes = 0;
  std::uint64_t compressed_bytes = 0;
  std::uint64_t speculative_bytes = 0;

  // Excludes free and speculative pages which the kernel can reclaim instantly.
  // This approximates "memory under real pressure" rather than literal byte
  // occupancy.
  [[nodiscard]] std::uint64_t used_bytes() const noexcept
  {
    return this->active_bytes + this->inactive_bytes + this->wired_bytes + this->compressed_bytes;
  }

  [[nodiscard]] double used_fraction() const noexcept
  {
    if (total_bytes == 0)
    {
      return 0.0;
    }
    return static_cast<double>(this->used_bytes()) / static_cast<double>(total_bytes);
  }
};
}  // namespace hostmon