#pragma once

#include <exe/fibers/sync/channel.hpp>

#include <variant>

// https://golang.org/src/runtime/chan.go
// https://golang.org/src/runtime/select.go

namespace exe::fibers {

namespace detail {

template <typename X, typename Y>
class Selector {
  using SelectedValue = std::variant<X, Y>;
  using MaybeSelectedValue = std::variant<X, Y, std::monostate>;

 public:
  SelectedValue Select(Channel<X>& /*xs*/, Channel<Y>& /*ys*/) {
    std::abort();  // Not implemented
  }

  MaybeSelectedValue TrySelect(Channel<X>& /*xs*/, Channel<Y>& /*ys*/) {
    std::abort();  // Not implemented
  }
};

}  // namespace detail

/*
 * Usage:
 * Channel<X> xs;
 * Channel<Y> ys;
 * ...
 * // value - std::variant<X, Y>
 * auto value = Select(xs, ys);
 * switch (value.index()) {
 *   case 0:
 *     // Handle std::get<0>(value);
 *     break;
 *   case 1:
 *     // Handle std::get<1>(value);
 *     break;
 * }
 */

template <typename X, typename Y>
auto Select(Channel<X>& xs, Channel<Y>& ys) {
  detail::Selector<X, Y> selector;
  return selector.Select(xs, ys);
}

/*
 * Usage:
 * Channel<X> xs;
 * Channel<Y> ys;
 * ...
 * // value - std::variant<X, Y, std::monostate>
 * auto value = TrySelect(xs, ys);
 * switch (value.index()) {
 *   case 0:
 *     // Handle std::get<0>(value);
 *     break;
 *   case 1:
 *     // Handle std::get<1>(value);
 *     break;
 *   default:
 *     // No value
 *     break;
 * }
 */

template <typename X, typename Y>
auto TrySelect(Channel<X>& xs, Channel<Y>& ys) {
  detail::Selector<X, Y> selector;
  return selector.TrySelect(xs, ys);
}

}  // namespace exe::fibers
