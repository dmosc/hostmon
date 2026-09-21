#include "hostmon/core/periodic_runner.h"

#include <algorithm>
#include <utility>

#include "hostmon/core/clock.h"

namespace hostmon
{

PeriodicRunner::PeriodicRunner(std::chrono::milliseconds interval, Task task)
    : interval_(interval),
      task_(std::move(task)),
      thread_([this](std::stop_token stop) { Run(std::move(stop)); })
{
}

void PeriodicRunner::Run(std::stop_token stop)
{
  auto next = Clock::now();
  while (!stop.stop_requested())
  {
    task_();
    // Schedule against an absolute deadline so task duration does not
    // accumulate as drift.
    next = std::max(next + interval_, Clock::now());
    std::unique_lock lock(mutex_);
    wake_.wait_until(lock, stop, next, [] { return false; });
  }
}

}  // namespace hostmon
