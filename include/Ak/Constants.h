#pragma once

#include <limits>

namespace Ak {

template<typename Scalar>
class Pi final
{
public:
  static constexpr Scalar value() noexcept { return 3.141592653589793238462643383; }
};

template<typename Scalar>
class Infinity final
{
public:
  static constexpr Scalar value() noexcept { return std::numeric_limits<Scalar>::infinity(); }
};

} // namespace Ak
