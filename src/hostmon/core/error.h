#pragma once

#include <expected>
#include <string>
#include <utility>

namespace hostmon
{
enum class ErrorCode
{
  kSystemCallFailed,
  kInconsistentSample,
};

struct Error
{
  ErrorCode code;
  std::string message;
};

template <typename T>
using Result = std::expected<T, Error>;

[[nodiscard]] inline std::unexpected<Error> MakeError(ErrorCode code, std::string message)
{
  return std::unexpected<Error>(Error{code, std::move(message)});
}
}  // namespace hostmon