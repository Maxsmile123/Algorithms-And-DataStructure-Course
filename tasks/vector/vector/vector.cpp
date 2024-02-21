#include "vector.hpp"

Vector::Vector() {
    // Not Implemented
}

Vector::Vector(size_t /*count*/, const T& /*value*/) {
    // Not Implemented
}

Vector::Vector(const Vector& /*other*/) {
    // Not Implemented
}

Vector& Vector::operator=(const Vector& /*other*/) {
    std::abort(); // Not Implemented
}

Vector::Vector(Vector&& /*other*/) noexcept {
    // Not Implemented
}

Vector::Vector(std::initializer_list<T> /*init*/) {
    // Not Implemented
}

T& Vector::operator[](size_t /*pos*/) {
    std::abort();  // Not Implemented
}

T& Vector::Front() const noexcept {
    std::abort();  // Not Implemented
}

bool Vector::IsEmpty() const noexcept {
    std::abort();  // Not Implemented
}

T& Vector::Back() const noexcept {
    std::abort();  // Not Implemented
}

T* Vector::Data() const noexcept {
    std::abort();  // Not Implemented
}

size_t Vector::Size() const noexcept {
    std::abort();  // Not Implemented
}

size_t Vector::Capacity() const noexcept {
    std::abort();  // Not Implemented
}

void Vector::Reserve(size_t /*new_cap*/) {
    // Not Implemented
}

void Vector::Clear() noexcept {
    // Not Implemented
}

void Vector::Insert(size_t /*pos*/, T /*value*/) {
    // Not Implemented
}

void Vector::Erase(size_t /*begin_pos*/, size_t /*end_pos*/) {
    // Not Implemented
}

void Vector::PushBack(T /*value*/) {
    // Not Implemented
}

template <class... Args>
void Vector::EmplaceBack(Args&&... /*args*/) {
    // Not Implemented
}

void Vector::PopBack() {
    // Not Implemented
}

void Vector::Resize(size_t /*count*/, const T& /*value*/) {
    // Not Implemented
}

Vector::~Vector() {
    // Not Implemented
}