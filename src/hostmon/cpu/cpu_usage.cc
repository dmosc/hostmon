#include "hostmon/cpu/cpu_usage.h"

#include <chrono>
#include <cstddef>
#include <cstdint>

namespace hostmon
{
namespace
{
struct TickDeltas
{
  std::uint64_t user = 0;
  std::uint64_t system = 0;
  std::uint64_t nice = 0;
  std::uint64_t idle = 0;

  [[nodiscard]] std::uint64_t total() const noexcept
  {
    return this->user + this->system + this->nice + this->idle;
  }

  TickDeltas& operator+=(const TickDeltas& other) noexcept
  {
    this->user += other.user;
    this->system += other.system;
    this->nice += other.nice;
    this->idle += other.idle;
    return *this;
  }
};

std::uint64_t CounterDelta(std::uint32_t previous, std::uint32_t current) noexcept
{
  return static_cast<std::uint32_t>(current - previous);
}

TickDeltas Delta(const CpuTicks& previous, const CpuTicks& current) noexcept
{
  return TickDeltas{
      .user = CounterDelta(previous.user, current.user),
      .system = CounterDelta(previous.system, current.system),
      .nice = CounterDelta(previous.nice, current.nice),
      .idle = CounterDelta(previous.idle, current.idle),
  };
}

CpuUsage ToUsage(const TickDeltas& deltas) noexcept
{
  const std::uint64_t total = deltas.total();
  // No elapsed ticks. Report fully idle.
  if (total == 0)
  {
    return CpuUsage{};
  }
  const auto denominator = static_cast<double>(total);
  return CpuUsage{
      .user = static_cast<double>(deltas.user) / denominator,
      .system = static_cast<double>(deltas.system) / denominator,
      .nice = static_cast<double>(deltas.nice) / denominator,
      .idle = static_cast<double>(deltas.idle) / denominator,
  };
}
}  // namespace

Result<CpuUsageSnapshot> ComputeUsage(const CpuTickSnapshot& previous,
                                      const CpuTickSnapshot& current)
{
  if (previous.cores.size() != current.cores.size())
  {
    return MakeError(ErrorCode::kInconsistentSample, "Core count changed between samples.");
  }
  CpuUsageSnapshot usage_snapshot;
  usage_snapshot.taken_at = current.taken_at;
  usage_snapshot.interval =
      std::chrono::duration_cast<std::chrono::nanoseconds>(current.taken_at - previous.taken_at);
  usage_snapshot.cores.reserve(current.cores.size());
  TickDeltas aggregate;
  for (std::size_t idx = 0; idx < current.cores.size(); idx++)
  {
    const TickDeltas deltas = Delta(previous.cores[idx], current.cores[idx]);
    aggregate += deltas;
    usage_snapshot.cores.push_back(ToUsage(deltas));
  }
  usage_snapshot.aggregate = ToUsage(aggregate);
  return usage_snapshot;
}
}  // namespace hostmon