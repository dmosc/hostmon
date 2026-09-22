#include "hostmon/platform/macos/mach_cpu_tick_source.h"

#include <mach/mach_error.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/processor_info.h>
#include <mach/vm_map.h>

#include <memory>
#include <span>
#include <string>

#include "hostmon/core/clock.h"

namespace hostmon
{
namespace
{
// host_processor_info() hands back a kernel-allocated buffer that we must
// vm_deallocate.
struct VmDeallocator
{
  mach_msg_type_number_t element_count;

  void operator()(processor_info_array_t buffer) const noexcept
  {
    vm_deallocate(mach_task_self(), reinterpret_cast<vm_address_t>(buffer),
                  element_count * sizeof(integer_t));
  }
};

using ProcessorInfoBuffer = std::unique_ptr<integer_t, VmDeallocator>;
}  // namespace

MachCpuTickSource::MachCpuTickSource() : host_(mach_host_self()) {}

MachCpuTickSource::~MachCpuTickSource() { mach_port_deallocate(mach_task_self(), host_); }

Result<CpuTickSnapshot> MachCpuTickSource::Read()
{
  natural_t core_count = 0;
  processor_info_array_t raw = nullptr;
  mach_msg_type_number_t raw_count = 0;

  const kern_return_t status =
      host_processor_info(host_, PROCESSOR_CPU_LOAD_INFO, &core_count, &raw, &raw_count);
  if (status != KERN_SUCCESS)
  {
    return MakeError(ErrorCode::kSystemCallFailed,
                     std::string("host_processor_info failed: ") + mach_error_string(status));
  }
  const ProcessorInfoBuffer buffer(raw, VmDeallocator{raw_count});

  if (raw_count < core_count * PROCESSOR_CPU_LOAD_INFO_COUNT)
  {
    return MakeError(ErrorCode::kSystemCallFailed, "host_processor_info returned a short buffer");
  }

  const std::span<const processor_cpu_load_info_data_t> loads(
      reinterpret_cast<const processor_cpu_load_info_data_t*>(buffer.get()), core_count);

  CpuTickSnapshot snapshot;
  snapshot.taken_at = Clock::now();
  snapshot.cores.reserve(loads.size());
  for (const auto& load : loads)
  {
    snapshot.cores.push_back(CpuTicks{
        .user = load.cpu_ticks[CPU_STATE_USER],
        .system = load.cpu_ticks[CPU_STATE_SYSTEM],
        .nice = load.cpu_ticks[CPU_STATE_NICE],
        .idle = load.cpu_ticks[CPU_STATE_IDLE],
    });
  }
  return snapshot;
}
}  // namespace hostmon
