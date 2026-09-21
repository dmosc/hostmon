#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stop_token>
#include <thread>

namespace hostmon
{
// Runs a task on a background thread at a fixed rate until destroyed.
class PeriodicRunner
{
 public:
  using Task = std::function<void()>;

  PeriodicRunner(std::chrono::milliseconds interval, Task task);

  PeriodicRunner(const PeriodicRunner&) = delete;

  PeriodicRunner& operator=(const PeriodicRunner&) = delete;

 private:
  std::chrono::milliseconds interval_;
  Task task_;
  std::mutex mutex_;
  std::condition_variable_any wake_;
  // Declared last so it is stopped and joined first.
  std::jthread thread_;

  void Run(std::stop_token stop);
};
}  // namespace hostmon