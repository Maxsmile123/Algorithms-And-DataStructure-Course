#include "fs.hpp"

namespace filesystem {

    void Fs::ChangeDir(const std::string& /*path*/) {
        // Not implemented
    }

    void Fs::ListFiles(const std::string& /*path = "."*/) const {
        // Not implemented
    }

    void Fs::MakeDir(
        const std::string& /*path*/,
        bool /*is_create_parents = false*/
    ) {
        // Not implemented
    }

    auto Fs::Split(const std::string& /*str*/, const std::string& /*splitter*/)
        -> std::vector<std::string>
    {
        std::abort(); // Not Implemented
    }

    void Fs::RemoveFile(const std::string& /*path*/) {
        // Not implemented
    }

    void Fs::CreateFile(
        const std::string& /*path*/, 
        bool /*is_overwrite = false*/
    ) {
        // Not implemented
    }

    void Fs::WriteToFile(
        const std::string& /*path*/, 
        bool /*is_overwrite = false*/,
        std::ostringstream& /*stream*/
    ) {
        // Not implemented
    }

    void Fs::ShowFileContent(const std::string& /*path*/) {
        // Not implemented
    }

    void Fs::FindFile(const std::string& /*filename*/) {
        // Not implemented
    }

    void Fs::PWD() const {
        // Not implemented
    }


} // end namespace filesystem