#pragma once

#include <common/ILegacyInputIterator.hpp>

// https://en.cppreference.com/w/cpp/named_req/Container
class IConteiner {
public:
    virtual IConteiner() noexcept = 0;
    virtual IConteiner(const IConteiner&) = 0;
    virtual IConteiner& operator=(const IConteiner&) = 0;

    virtual bool operator==(const IConteiner&, const IConteiner&) = 0;
    virtual bool operator!=(const IConteiner&, const IConteiner&) = 0;

    virtual ~IConteiner() noexcept = 0;

    virtual void swap(IConteiner&) = 0;

    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
}