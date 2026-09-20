#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace hostmon
{
// Fixed-capacity FIFO that overwrites the oldest element once full. Requires
// T to be default-constructible. Not thread safe.
template <typename T>
class RingBuffer
{
 public:
  explicit RingBuffer(std::size_t capacity) : buffer_(capacity)
  {
    if (capacity <= 0)
    {
      throw std::invalid_argument("RingBuffer capacity must be greater than 0.");
    }
  }

  void Push(T value)
  {
    if (this->size_ < buffer_.size())
    {
      this->buffer_[(this->head_ + this->size_) % this->buffer_.size()] = std::move(value);
      this->size_++;
    }
    else
    {
      this->buffer_[this->head_] = std::move(value);
      this->head_ = (this->head_ + 1) % this->buffer_.size();
    }
  }

  [[nodiscard]] const T& operator[](std::size_t idx) const
  {
    return this->buffer_[(this->head_ + idx) % this->buffer_.size()];
  }

  [[nodiscard]] std::vector<T> ToVector() const
  {
    std::vector<T> buffer;
    buffer.reserve(this->size_);
    for (std::size_t idx = 0; idx < this->size_; idx++)
    {
      buffer.push_back((*this)[idx]);
    }
    return buffer;
  }

  [[nodiscard]] std::size_t size() const noexcept { return this->size_; }
  [[nodiscard]] std::size_t capacity() const noexcept { return this->buffer_.size(); }
  [[nodiscard]] bool empty() const noexcept { return this->size_ == 0; }

 private:
  std::vector<T> buffer_;
  std::size_t head_ = 0;
  std::size_t size_ = 0;
};
}  // namespace hostmon