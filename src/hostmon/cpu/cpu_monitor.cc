#include "hostmon/cpu/cpu_monitor.h"

#include <utility>

#include "hostmon/cpu/cpu_usage.h"

namespace hostmon
{
CpuMonitor::CpuMonitor(std::unique_ptr<CpuTickSource> source, std::size_t history_capacity)
    : source_(std::move(source)), busy_history_(history_capacity)
{
}

Result<void> CpuMonitor::Poll()
{
  auto current = source_->Read();
  if (!current)
  {
    return std::unexpected(std::move(current.error()));
  }

  if (!previous_)
  {
    previous_ = std::move(*current);
    return {};
  }

  auto usage = ComputeUsage(*previous_, *current);
  previous_ = std::move(*current);
  if (!usage)
  {
    return std::unexpected(std::move(usage.error()));
  }

  const std::lock_guard lock(mutex_);
  busy_history_.Push(usage->aggregate.busy());
  latest_ = std::move(*usage);
  return {};
}

std::optional<CpuUsageSnapshot> CpuMonitor::Latest() const
{
  const std::lock_guard lock(this->mutex_);
  return this->latest_;
}

std::vector<double> CpuMonitor::BusyHistory() const
{
  const std::lock_guard lock(this->mutex_);
  return this->busy_history_.ToVector();
}
}  // namespace hostmon
