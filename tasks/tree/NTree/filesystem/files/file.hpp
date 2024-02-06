#pragma once

#include <string>


namespace filesystem {

class File {
public:
    void Read(size_t /*bytes*/) const;

    // overwrite
    void Write();

    void Append();

private:
    std::string content;
};

} // end namespace filesystem