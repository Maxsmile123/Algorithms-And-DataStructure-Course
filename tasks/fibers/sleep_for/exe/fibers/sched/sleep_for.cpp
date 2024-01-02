#include <exe/fibers/sched/sleep_for.hpp>

#include <asio/steady_timer.hpp>

namespace exe::fibers {

void SleepFor(Millis delay) {
  auto io = *(Fiber::Self()->scheduler_);
  asio::steady_timer timer{io};

  timer.expires_after(delay);

  asio::post(io, [&io, &timer](){
    timer.async_wait([](const std::error_code&) {
      Yield();
    });
  });
}

}  // namespace exe::fibers
