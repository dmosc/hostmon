#pragma once

#include "hostmon/core/error.h"
#include "hostmon/cpu/cpu_types.h"

namespace hostmon
{
// Converts two cumulative tick snapshots into per-core and aggregate usage over
// the interval between them.
[[nodiscard]] Result<CpuUsageSnapshot> ComputeUsage(const CpuTickSnapshot& previous,
                                                    const CpuTickSnapshot& current);
}  // namespace hostmon