#pragma once

#include "hostmon/core/error.h"
#include "hostmon/memory/memory_types.h"

namespace hostmon
{
class MemorySampleSource
{
 public:
  MemorySampleSource() = default;

  virtual ~MemorySampleSource() = default;

  MemorySampleSource(const MemorySampleSource&) = delete;

  MemorySampleSource& operator=(const MemorySampleSource&) = delete;

  MemorySampleSource(MemorySampleSource&&) = delete;

  MemorySampleSource& operator=(MemorySampleSource&&) = delete;

  [[nodiscard]] virtual Result<MemorySample> Read() = 0;
};
}  // namespace hostmon