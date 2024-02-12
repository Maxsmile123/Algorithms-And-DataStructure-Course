#include "file.hpp"

namespace filesystem {

    void File::Read(size_t /*bytes*/) const {
        std::abort(); // Not implemented
    }

    void File::Write() {
        // Not implemented
    }

    void File::Append() {
        // Not implemented
    }

} // end namespace filesystem