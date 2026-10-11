#pragma once

#include "hostmon/core/error.h"
#include "hostmon/memory/memory_types.h"

namespace hostmon
{
// Converts a raw page-count sample into byte-denominated usage. Pure function.
[[nodiscard]] Result<MemoryUsage> ComputeMemoryUsage(const MemorySample& sample);
}  // namespace hostmon
