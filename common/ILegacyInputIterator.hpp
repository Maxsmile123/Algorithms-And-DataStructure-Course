#pragma once

#include <iterator>

// https://en.cppreference.com/w/cpp/named_req/InputIterator
template<typename ValueType>
class IIterator: public std::iterator<std::input_iterator_tag, ValueType> {
    friend IConteiner;
private:
    explicit IIterator(ValueType*) = 0;
public:

    IIterator(const IIterator&) = 0;

    virtual bool operator==(const IIterator&, const IIterator&) const = 0;
    virtual bool operator!=(const IIterator&, const IIterator&) const = 0;

    /*
    The overload of operator -> must either return a raw pointer, 
    or return an object (by reference or by value) for which 
    operator -> is in turn overloaded.
    */
    virtual typename IIterator::pointer operator->() const = 0;
    virtual typename IIterator::reference operator*() const = 0;

    virtual IIterator& operator++() = 0;
    virtual IIterator operator++(int) = 0;

private:
    ValueType* p;

}

