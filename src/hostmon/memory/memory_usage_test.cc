#include "hostmon/memory/memory_usage.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace hostmon
{
using ::testing::DoubleEq;

namespace
{
TEST(ComputeMemoryUsageTest, ConverstPagesToBytes)
{
  const MemorySample sample{
      .taken_at = Clock::time_point{},
      .page_size = 16384,
      .pages =
          VmPageCounts{
              .free = 10,
              .active = 20,
              .inactive = 5,
              .wired = 15,
              .compressed = 0,
              .speculative = 0,
          },
      .physical_total_bytes = 50 * 16384,
  };
  const auto usage = ComputeMemoryUsage(sample);
  ASSERT_TRUE(usage.has_value());
  EXPECT_EQ(usage->active_bytes, 20u * 16384u);
  EXPECT_EQ(usage->used_bytes(), (20u + 5u + 15u) * 16384u);
  EXPECT_THAT(usage->used_fraction(), DoubleEq(40.0 / 50.0));
}

TEST(ComputeMemoryUsageTest, RejectsZeroPageSize)
{
  const MemorySample sample{
      .taken_at = Clock::time_point{},
      .page_size = 0,
  };
  const auto usage = ComputeMemoryUsage(sample);
  ASSERT_FALSE(usage.has_value());
  EXPECT_EQ(usage.error().code, ErrorCode::kInconsistentSample);
}
}  // namespace
}  // namespace hostmon