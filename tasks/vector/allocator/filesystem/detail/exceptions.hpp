#pragma once

#include <exception>
#include <string>

#include <fmt/core.h>

namespace filesystem::exceptions {

class FileNotFoundException: std::exception {
public:
    explicit FileNotFoundException(const std::string& filename):
        error_message_(filename){}

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};


} // end namespace filesystem::exceptions