#pragma once

#include <mach/port.h>

#include "hostmon/core/error.h"
#include "hostmon/cpu/cpu_tick_source.h"
#include "hostmon/cpu/cpu_types.h"

namespace hostmon
{
// Reads per-core CPU tick counters via the Mach host_processor_info() call.
class MachCpuTickSource final : public CpuTickSource
{
 public:
  MachCpuTickSource();

  ~MachCpuTickSource() override;

  [[nodiscard]] Result<CpuTickSnapshot> Read() override;

 private:
  mach_port_t host_;
};
}  // namespace hostmon