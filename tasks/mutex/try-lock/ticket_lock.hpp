#pragma once

#include <twist/ed/stdlike/atomic.hpp>
#include <twist/ed/wait/spin.hpp>

#include <cstdlib>

class TicketLock {
  using Ticket = uint64_t;

 public:
  // Do not change this method
  void Lock() {
    const Ticket this_thread_ticket = next_free_ticket_.fetch_add(1);

    twist::ed::SpinWait spin_wait;
    while (this_thread_ticket != owner_ticket_.load()) {
      spin_wait();
    }
  }

  bool TryLock() {
    Ticket owner = owner_ticket_.load();
    Ticket this_thread_ticket = next_free_ticket_.load();
    return next_free_ticket_.compare_exchange_strong(owner, ++this_thread_ticket);  

  }

  // Do not change this method
  void Unlock() {
    // Do we actually need atomic increment here?
    owner_ticket_.fetch_add(1);
  }

 private:
  twist::ed::stdlike::atomic<Ticket> next_free_ticket_{0};
  twist::ed::stdlike::atomic<Ticket> owner_ticket_{0};
};
