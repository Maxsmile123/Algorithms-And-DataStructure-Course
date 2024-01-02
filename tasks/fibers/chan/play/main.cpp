#include <exe/executors/thread_pool.hpp>

#include <exe/fibers/core/api.hpp>
#include <exe/fibers/sync/channel.hpp>

#include <fmt/core.h>

#include <mutex>

using namespace exe;

static const int kPoisonPill = -1;

int main() {
  executors::ThreadPool scheduler{/*threads=*/4};

  fibers::Go(scheduler, []() {
    fibers::Channel<int> msgs{16};

    // Producer
    fibers::Go([msgs]() mutable {
      for (int i = 0; i < 128; ++i) {
        msgs.Send(i);
      }

      msgs.Send(kPoisonPill);
    });

    // Consumer
    while (true) {
      int value = msgs.Receive();
      if (value == kPoisonPill) {
        break;
      }
      fmt::println("Received value = {}", value);
    }
  });

  scheduler.WaitIdle();
  scheduler.Stop();

  return 0;
}
