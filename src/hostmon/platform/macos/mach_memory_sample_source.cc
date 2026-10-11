#include "hostmon/platform/macos/mach_memory_sample_source.h"

#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_port.h>
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>

#include <cstddef>
#include <string>

#include "hostmon/core/clock.h"

namespace hostmon
{
namespace
{
Result<std::uint64_t> PhysicalMemoryBytes()
{
  std::uint64_t bytes = 0;
  std::size_t size = sizeof(bytes);
  if (sysctlbyname("hw.memsize", &bytes, &size, nullptr, 0) != 0)
  {
    return MakeError(ErrorCode::kSystemCallFailed, "sysctlbyname(hw.memsize) failed");
  }
  return bytes;
}
}  // namespace

MachMemorySampleSource::MachMemorySampleSource() : host_(mach_host_self()), physical_total_bytes_(0)
{
  if (auto bytes = PhysicalMemoryBytes())
  {
    this->physical_total_bytes_ = *bytes;
  }
};

MachMemorySampleSource::~MachMemorySampleSource()
{
  mach_port_deallocate(mach_task_self(), this->host_);
};

Result<MemorySample> MachMemorySampleSource::Read()
{
  vm_size_t page_size = 0;
  if (host_page_size(this->host_, &page_size) != KERN_SUCCESS)
  {
    return MakeError(ErrorCode::kSystemCallFailed, "host_page_size failed");
  }
  vm_statistics64_data_t stats{};
  mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
  const kern_return_t status = host_statistics64(this->host_, HOST_VM_INFO64,
                                                 reinterpret_cast<host_info64_t>(&stats), &count);
  if (status != KERN_SUCCESS)
  {
    return MakeError(ErrorCode::kSystemCallFailed, "host_statistics64 failed");
  }
  return MemorySample{
      .taken_at = Clock::now(),
      .page_size = page_size,
      .pages =
          VmPageCounts{
              .free = stats.free_count,
              .active = stats.active_count,
              .inactive = stats.inactive_count,
              .wired = stats.wire_count,
              .compressed = stats.compressor_page_count,
              .speculative = stats.speculative_count,
          },
      .physical_total_bytes = physical_total_bytes_,
  };
}
}  // namespace hostmon