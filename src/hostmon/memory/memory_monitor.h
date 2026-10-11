#pragma once

#include <cstddef>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include "hostmon/core/error.h"
#include "hostmon/core/ring_buffer.h"
#include "hostmon/memory/memory_sample_source.h"
#include "hostmon/memory/memory_types.h"

namespace hostmon
{
// Owns sampling state for memory metrics.
class MemoryMonitor
{
 public:
  MemoryMonitor(std::unique_ptr<MemorySampleSource> source, std::size_t history_capacity);

  Result<void> Poll();

  [[nodiscard]] std::optional<MemoryUsage> Latest() const;

  [[nodiscard]] std::vector<double> UsedHistory() const;

 private:
  std::unique_ptr<MemorySampleSource> source_;

  mutable std::mutex mutex_;

  std::optional<MemoryUsage> latest_;

  RingBuffer<double> used_history_;
};
}  // namespace hostmon