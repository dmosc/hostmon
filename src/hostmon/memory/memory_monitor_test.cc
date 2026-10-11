#include "hostmon/memory/memory_monitor.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <utility>

namespace hostmon
{
namespace
{
using ::testing::DoubleEq;
using ::testing::ElementsAre;

class FakeMemorySampleSource final : public MemorySampleSource
{
 public:
  void Enqueue(Result<MemorySample> result) { this->script_.push_back(std::move(result)); }

  Result<MemorySample> Read() override
  {
    if (this->script_.empty())
    {
      return MakeError(ErrorCode::kSystemCallFailed, "script_ exhausted");
    }
    auto next_sample = std::move(this->script_.front());
    this->script_.pop_front();
    return next_sample;
  }

 private:
  std::deque<Result<MemorySample>> script_;
};

MemorySample Sample(std::uint64_t used_pages, std::uint64_t free_pages)
{
  return MemorySample{
      .taken_at = Clock::time_point{},
      .page_size = 1,
      .pages =
          VmPageCounts{
              .free = free_pages,
              .active = used_pages,
          },
      .physical_total_bytes = used_pages + free_pages,
  };
}

TEST(MemoryMonitorTest, FirstPollPublishesImmediatly)
{
  auto source = std::make_unique<FakeMemorySampleSource>();
  source->Enqueue(Sample(30, 70));
  MemoryMonitor monitor(std::move(source), 4);
  ASSERT_TRUE(monitor.Poll().has_value());
  ASSERT_TRUE(monitor.Latest().has_value());
  EXPECT_THAT(monitor.Latest()->used_fraction(), DoubleEq(0.30));
  EXPECT_THAT(monitor.UsedHistory(), ElementsAre(DoubleEq(0.30)));
}

TEST(MemoryMonitorTest, PropagatesSourceErrors)
{
  auto source = std::make_unique<FakeMemorySampleSource>();
  source->Enqueue(MakeError(ErrorCode::kSystemCallFailed, "boom"));
  MemoryMonitor monitor(std::move(source), 4);
  const auto result = monitor.Poll();
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error().message, "boom");
}
}  // namespace
}  // namespace hostmon