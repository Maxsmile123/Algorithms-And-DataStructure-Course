#include <stdlike/promise.hpp>

#include <fmt/core.h>

#include <string>
#include <thread>

int main() {
  // Contract
  stdlike::Promise<std::string> p;
  auto f = p.MakeFuture();

  // Producer
  std::thread producer([p = std::move(p)]() mutable {
    p.SetValue("Hello");
  });

  // Consumer
  fmt::println("Value = {}", f.Get());

  producer.join();

  return 0;
}
