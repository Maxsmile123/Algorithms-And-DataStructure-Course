#include "vector.hpp"

template <typename T>
Vector<T>::Vector() {
    // Not Implemented
}

template <typename T>
Vector<T>::Vector(size_t /*count*/, const T& /*value*/) {
    // Not Implemented
}

template <typename T>
Vector<T>::Vector(const Vector& /*other*/) {
    // Not Implemented
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector& /*other*/) {
    std::abort();  // Not Implemented
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& /*other*/) {
    std::abort();  // Not Implemented
}

template <typename T>
Vector<T>::Vector(Vector&& /*other*/) noexcept {
    // Not Implemented
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> /*init*/) {
    // Not Implemented
}

template <typename T>
T& Vector<T>::operator[](size_t /*pos*/) {
    std::abort();  // Not Implemented
}

template <typename T>
T& Vector<T>::Front() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
bool Vector<T>::IsEmpty() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
T& Vector<T>::Back() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
T* Vector<T>::Data() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
size_t Vector<T>::Size() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
size_t Vector<T>::Capacity() const noexcept {
    std::abort();  // Not Implemented
}

template <typename T>
void Vector<T>::Reserve(size_t /*new_cap*/) {
    // Not Implemented
}

template <typename T>
void Vector<T>::Clear() noexcept {
    // Not Implemented
}

template <typename T>
void Vector<T>::Insert(size_t /*pos*/, T /*value*/) {
    // Not Implemented
}

template <typename T>
void Vector<T>::Erase(size_t /*begin_pos*/, size_t /*end_pos*/) {
    // Not Implemented
}

template <typename T>
void Vector<T>::PushBack(T /*value*/) {
    // Not Implemented
}

template <typename T>
template <class... Args>
void Vector<T>::EmplaceBack(Args&&... /*args*/) {
    // Not Implemented
}

template <typename T>
void Vector<T>::PopBack() {
    // Not Implemented
}

template <typename T>
void Vector<T>::Resize(size_t /*count*/, const T& /*value*/) {
    // Not Implemented
}

template <typename T>
Vector<T>::~Vector() {
    // Not Implemented
}