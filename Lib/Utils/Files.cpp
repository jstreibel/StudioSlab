//
// Created by joao on 30/03/24.
//

#include "Files.h"
#include "String.h"

#include <filesystem>

void Utils::CheckFolderExists(const Str &dirPath) {
    namespace fs = std::filesystem;

    if (!fs::exists(dirPath)) {
        if (!fs::create_directories(dirPath))
            throw fs::filesystem_error(Str("Failed creating path '") + dirPath +"'.",
                                       dirPath,
                                       std::make_error_code(std::errc::permission_denied));
    }
}
