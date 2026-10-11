#include "hostmon/memory/memory_usage.h"

namespace hostmon
{
Result<MemoryUsage> ComputeMemoryUsage(const MemorySample& sample)
{
  if (sample.page_size == 0)
  {
    return MakeError(ErrorCode::kInconsistentSample, "memory sample has zero page size");
  }
  const auto to_bytes = [&](std::uint64_t pages) { return pages * sample.page_size; };
  return MemoryUsage{
      .total_bytes = sample.physical_total_bytes,
      .free_bytes = to_bytes(sample.pages.free),
      .active_bytes = to_bytes(sample.pages.active),
      .inactive_bytes = to_bytes(sample.pages.inactive),
      .wired_bytes = to_bytes(sample.pages.wired),
      .compressed_bytes = to_bytes(sample.pages.compressed),
      .speculative_bytes = to_bytes(sample.pages.speculative),
  };
}
}  // namespace hostmon