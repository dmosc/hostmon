#include "hostmon/memory/memory_monitor.h"

#include <utility>

#include "hostmon/memory/memory_usage.h"

namespace hostmon
{
MemoryMonitor::MemoryMonitor(std::unique_ptr<MemorySampleSource> source,
                             std::size_t history_capacity)
    : source_(std::move(source)), used_history_(history_capacity) {};

Result<void> MemoryMonitor::Poll()
{
  auto sample = this->source_->Read();
  if (!sample)
  {
    return std::unexpected(std::move(sample.error()));
  }
  auto usage = ComputeMemoryUsage(*sample);
  if (!usage)
  {
    return std::unexpected(std::move(usage.error()));
  }
  const std::lock_guard lock(this->mutex_);
  this->used_history_.Push(usage->used_fraction());
  this->latest_ = std::move(*usage);
  return {};
};

std::optional<MemoryUsage> MemoryMonitor::Latest() const
{
  const std::lock_guard lock(this->mutex_);
  return this->latest_;
};

std::vector<double> MemoryMonitor::UsedHistory() const
{
  const std::lock_guard lock(this->mutex_);
  return this->used_history_.ToVector();
};
}  // namespace hostmon
