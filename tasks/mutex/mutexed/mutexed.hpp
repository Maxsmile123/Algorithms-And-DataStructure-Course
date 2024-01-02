#pragma once

#include <twist/ed/stdlike/mutex.hpp>

//////////////////////////////////////////////////////////////////////





template <typename T, class Mutex = twist::ed::stdlike::mutex>
class Mutexed {
  class OwnerRef{
   public:
    explicit OwnerRef(Mutexed* resource): resource_(resource){
              resource->mutex_.lock();
    }

    OwnerRef(const OwnerRef&) = delete;
    void operator=(T const&) = delete;

    T* operator->(){
      return &resource_->object_;
    }

    T* operator*(){
      return &resource_->object_;
    }
    ~OwnerRef(){
      resource_->mutex_.unlock();
    }
   private:
    Mutexed* resource_;
  };

 public:
  template <typename... Args>
  explicit Mutexed(Args&&... args)
      : object_(std::forward<Args>(args)...) {
  }
  Mutexed(const Mutexed&) = delete;
  void operator=(Mutexed const&) = delete;
  Mutexed(const Mutexed&&) = delete;

  OwnerRef Acquire() {
    return OwnerRef(this);
  };


 private:
  T object_;
  Mutex mutex_;
};
//////////////////////////////////////////////////////////////////////

template <typename T>
auto Acquire(Mutexed<T>& object) {
  return object.Acquire();
}
