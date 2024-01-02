#pragma once

#include <common/IIterativelyConteiner.hpp>

class IMoveOnlyConteiner: public IIterativelyConteiner {
public:
    IConteiner(const IConteiner&) = delete;
    IConteiner& operator=(const IConteiner&) = delete;

    virtual IConteiner(IConteiner&&) = 0;
    virtual IConteiner& operator=(IConteiner&&) = 0;
}