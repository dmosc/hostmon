#pragma once

#include "hostmon/core/error.h"
#include "hostmon/cpu/cpu_types.h"

namespace hostmon
{
class CpuTickSource
{
 public:
  CpuTickSource() = default;

  virtual ~CpuTickSource() = default;

  CpuTickSource(const CpuTickSource&) = delete;

  CpuTickSource& operator=(const CpuTickSource&) = delete;

  CpuTickSource(CpuTickSource&&) = delete;

  CpuTickSource& operator=(CpuTickSource&&) = delete;

  [[nodiscard]] virtual Result<CpuTickSnapshot> Read() = 0;
};
}  // namespace hostmon