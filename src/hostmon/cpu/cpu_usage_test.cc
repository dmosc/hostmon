#include "hostmon/cpu/cpu_usage.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace hostmon
{
namespace
{
using ::testing::DoubleEq;

CpuTickSnapshot MakeSnapshot(std::vector<CpuTicks> cores)
{
  return CpuTickSnapshot{.taken_at = Clock::time_point{}, .cores = std::move(cores)};
}

TEST(ComputeUsageTest, SplitsIntervalAcrossStates)
{
  const auto previous = MakeSnapshot({CpuTicks{}});
  const auto current = MakeSnapshot({CpuTicks{.user = 25, .system = 25, .idle = 50}});
  const auto usage = ComputeUsage(previous, current);
  ASSERT_TRUE(usage.has_value());
  EXPECT_THAT(usage->aggregate.user, DoubleEq(0.25));
  EXPECT_THAT(usage->aggregate.system, DoubleEq(0.25));
  EXPECT_THAT(usage->aggregate.idle, DoubleEq(0.50));
  EXPECT_THAT(usage->aggregate.busy(), DoubleEq(0.50));
}

TEST(ComputeUsageTest, AggregateWeightsCoresByTicks)
{
  const auto previous = MakeSnapshot({CpuTicks{}, CpuTicks{}});
  const auto current = MakeSnapshot({CpuTicks{.user = 100}, CpuTicks{.idle = 100}});
  const auto usage = ComputeUsage(previous, current);
  ASSERT_TRUE(usage.has_value());
  EXPECT_THAT(usage->cores[0].busy(), DoubleEq(1.0));
  EXPECT_THAT(usage->cores[1].busy(), DoubleEq(0.0));
  EXPECT_THAT(usage->aggregate.busy(), DoubleEq(0.5));
}

TEST(ComputeUsageTest, HandlesCounterWraparound)
{
  constexpr std::uint32_t kMax = std::numeric_limits<std::uint32_t>::max();
  const auto previous = MakeSnapshot({CpuTicks{.user = kMax - 9}});
  const auto current = MakeSnapshot({CpuTicks{.user = 10, .idle = 20}});
  const auto usage = ComputeUsage(previous, current);
  ASSERT_TRUE(usage.has_value());
  EXPECT_THAT(usage->aggregate.user, DoubleEq(0.5));
}

TEST(ComputeUsageTest, ZeroElapsedTicksReportsIdle)
{
  const auto snapshot = MakeSnapshot({CpuTicks{.user = 7}});
  const auto usage = ComputeUsage(snapshot, snapshot);
  ASSERT_TRUE(usage.has_value());
  EXPECT_THAT(usage->aggregate.idle, DoubleEq(1.0));
}

TEST(ComputeUsageTest, RejectsChangedCoreCount)
{
  const auto usage =
      ComputeUsage(MakeSnapshot({CpuTicks{}}), MakeSnapshot({CpuTicks{}, CpuTicks{}}));
  ASSERT_FALSE(usage.has_value());
  EXPECT_EQ(usage.error().code, ErrorCode::kInconsistentSample);
}
}  // namespace
}  // namespace hostmon