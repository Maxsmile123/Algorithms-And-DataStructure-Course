#pragma once

#include <common/IContainer.hpp>
#include <common/ILegacyInputIterator.hpp>

class IIterativelyConteiner: public IContainer {
public:
    virtual IIterator begin() const = 0;
    virtual IIterator end() const = 0;
}