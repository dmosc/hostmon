#pragma once

#include <mach/port.h>

#include "hostmon/core/error.h"
#include "hostmon/memory/memory_sample_source.h"
#include "hostmon/memory/memory_types.h"

namespace hostmon
{
class MachMemorySampleSource final : public MemorySampleSource
{
 public:
  MachMemorySampleSource();

  ~MachMemorySampleSource() override;

  [[nodiscard]] Result<MemorySample> Read() override;

 private:
  mach_port_t host_;
  // Fixed at boot.
  std::uint64_t physical_total_bytes_;
};
}  // namespace hostmon