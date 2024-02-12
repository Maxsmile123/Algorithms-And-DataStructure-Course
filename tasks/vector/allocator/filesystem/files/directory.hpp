#pragma once

#include <vector>

#include "file.hpp"
#include "../map/map.hpp"
#include "../fs.hpp"


namespace filesystem {

class Directory {
friend class Fs;

    void GetName() const;

private:
    Directory* parent;
    Map<std::string, Directory*> childs;
    Map<std::string, File> files;
};

} // end namespace filesystem