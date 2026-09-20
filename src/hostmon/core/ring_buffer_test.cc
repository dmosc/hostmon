#include "hostmon/core/ring_buffer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <stdexcept>

namespace hostmon
{
namespace
{
using ::testing::ElementsAre;
TEST(RingBufferTest, StartsEmpty)
{
  const RingBuffer<int> buffer(3);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0U);
  EXPECT_EQ(buffer.capacity(), 3U);
}

TEST(RingBufferTest, KeepsInsertionOrderBelowCapacity)
{
  RingBuffer<int> buffer(3);
  buffer.Push(1);
  buffer.Push(2);
  EXPECT_THAT(buffer.ToVector(), ElementsAre(1, 2));
}

TEST(RingBufferTest, OverwritesOldestWhenFull)
{
  RingBuffer<int> buffer(3);
  for (int idx = 0; idx <= 5; idx++)
  {
    buffer.Push(idx);
  }
  EXPECT_EQ(buffer.size(), 3U);
  EXPECT_THAT(buffer.ToVector(), ElementsAre(3, 4, 5));
}

TEST(RingBufferTest, ZeroCapacityIsRejected)
{
  EXPECT_THROW(RingBuffer<int>(0), std::invalid_argument);
}
}  // namespace
}  // namespace hostmon