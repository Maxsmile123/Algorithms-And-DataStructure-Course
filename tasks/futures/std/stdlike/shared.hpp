
#include <twist/ed/stdlike/atomic.hpp>

#include <memory>

namespace stdlike::detail{

template <typename T>
class Shared{
 public:   
    Shared(){
    value_ = std::make_shared<std::variant<T, std::exception_ptr>>(std::exception_ptr(nullptr));
    is_set_ = std::make_shared<twist::ed::stdlike::atomic<bool>>(false);
 }
    // Non-copyable
  Shared(const Shared&) = delete;
  Shared& operator=(const Shared&) = delete;

  // Movable
  Shared(Shared&&) = default;
  Shared& operator=(Shared&&) = default;

  // One-shot
  // Wait for result (value or exception)
  T GetValue() {
    while(!(is_set_->load())){};
    try{
      std::rethrow_exception(std::get<std::exception_ptr>(*value_));
    } catch(std::bad_variant_access&){
      return std::move(std::get<T>(*value_));
    }
  }

  void SetValue(T value){
    *value_ = std::move(value);
    is_set_->store(true);
  }

  void SetExc(std::exception_ptr value){
    *value_ = value;
    is_set_->store(true);
  }

 private:
  std::shared_ptr<std::variant<T, std::exception_ptr>> value_;
  std::shared_ptr<twist::ed::stdlike::atomic<bool>> is_set_;
};

template <>
class Shared<std::exception_ptr>{
 public:

 Shared(){
    value_ = std::make_shared<std::exception_ptr>(nullptr);
    is_set_value_ = std::make_shared<twist::ed::stdlike::atomic<bool>>(false);
    is_set_exc_ = std::make_shared<twist::ed::stdlike::atomic<bool>>(false);
 }
// Non-copyable
  Shared(const Shared&) = delete;
  Shared& operator=(const Shared&) = delete;

  // Movable
  Shared(Shared&&) = default;
  Shared& operator=(Shared&&) = default;

  // One-shot
  // Wait for result (value or exception)
  std::exception_ptr GetValue() {
    while(!(is_set_value_->load() || is_set_exc_->load())){};
    if (is_set_value_->load()){
      return *value_;
    } else{
      std::rethrow_exception(*value_);
    }
  }

  void SetValue(std::exception_ptr value){
    *value_ = std::move(value);
    is_set_value_->store(true);
  }

  void SetExc(std::exception_ptr value){
    *value_ = std::move(value);
    is_set_exc_->store(true);
  }

 private:
  std::shared_ptr<std::exception_ptr> value_;
  std::shared_ptr<twist::ed::stdlike::atomic<bool>> is_set_value_;
  std::shared_ptr<twist::ed::stdlike::atomic<bool>> is_set_exc_;
};

}