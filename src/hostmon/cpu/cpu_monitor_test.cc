#include "hostmon/cpu/cpu_monitor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <deque>
#include <memory>
#include <utility>

namespace hostmon
{
namespace
{
using ::testing::DoubleEq;
using ::testing::ElementsAre;

class FakeCpuTickSource final : public CpuTickSource
{
 public:
  void Enqueue(Result<CpuTickSnapshot> result) { this->script_.push_back(std::move(result)); }

  Result<CpuTickSnapshot> Read() override
  {
    if (this->script_.empty())
    {
      return MakeError(ErrorCode::kSystemCallFailed, "FakeCpuTickSource script exhausted.");
    }
    auto next_tick = std::move(this->script_.front());
    this->script_.pop_front();
    return next_tick;
  }

 private:
  std::deque<Result<CpuTickSnapshot>> script_;
};

CpuTickSnapshot Snapshot(std::uint32_t user, std::uint32_t idle)
{
  return CpuTickSnapshot{.taken_at = Clock::time_point{},
                         .cores = {CpuTicks{.user = user, .idle = idle}}};
}

TEST(CpuMonitorTest, FirstPollOnlyRecordsBaseline)
{
  auto source = std::make_unique<FakeCpuTickSource>();
  source->Enqueue(Snapshot(0, 0));
  CpuMonitor monitor(std::move(source), 4);
  ASSERT_TRUE(monitor.Poll().has_value());
  EXPECT_FALSE(monitor.Latest().has_value());
  EXPECT_TRUE(monitor.BusyHistory().empty());
}

TEST(CpuMonitorTest, SecondPollPublishesUsageAndHistory)
{
  auto source = std::make_unique<FakeCpuTickSource>();
  source->Enqueue(Snapshot(0, 0));
  source->Enqueue(Snapshot(50, 50));
  source->Enqueue(Snapshot(150, 50));
  CpuMonitor monitor(std::move(source), 4);

  ASSERT_TRUE(monitor.Poll().has_value());
  ASSERT_TRUE(monitor.Poll().has_value());
  ASSERT_TRUE(monitor.Poll().has_value());

  ASSERT_TRUE(monitor.Latest().has_value());
  EXPECT_THAT(monitor.Latest()->aggregate.busy(), DoubleEq(1.0));
  EXPECT_THAT(monitor.BusyHistory(), ElementsAre(DoubleEq(0.5), DoubleEq(1.0)));
}

TEST(CpuMonitorTest, PropagatesSourceErrors)
{
  auto source = std::make_unique<FakeCpuTickSource>();
  source->Enqueue(MakeError(ErrorCode::kSystemCallFailed, "boom"));
  CpuMonitor monitor(std::move(source), 4);

  const auto result = monitor.Poll();

  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().message, "boom");
}
}  // namespace
}  // namespace hostmon