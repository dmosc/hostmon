#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include "hostmon/core/error.h"
#include "hostmon/core/ring_buffer.h"
#include "hostmon/cpu/cpu_tick_source.h"
#include "hostmon/cpu/cpu_types.h"

namespace hostmon
{
// Owns the sampling state for CPU metrics.
class CpuMonitor
{
 public:
  CpuMonitor(std::unique_ptr<CpuTickSource> source, std::size_t history_capacity);

  // Reads a new sample. First successful call only records a baseline.
  Result<void> Poll();

  [[nodiscard]] std::optional<CpuUsageSnapshot> Latest() const;

  // Aggregate busy fraction per sample.
  [[nodiscard]] std::vector<double> BusyHistory() const;

 private:
  std::unique_ptr<CpuTickSource> source_;
  std::optional<CpuTickSnapshot> previous_;
  // Shared between sample and readers; guarded by mutex_.
  mutable std::mutex mutex_;
  std::optional<CpuUsageSnapshot> latest_;
  RingBuffer<double> busy_history_;
};
}  // namespace hostmon