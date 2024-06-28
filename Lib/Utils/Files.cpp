//
// Created by joao on 30/03/24.
//

#include "Files.h"
#include "String.h"

#include <filesystem>

void Slab::Utils::TouchFolder(const Str &folder) {
    namespace fs = std::filesystem;

    if (!fs::exists(folder)) {
        if (!fs::create_directories(folder))
            throw fs::filesystem_error(Str("Failed creating path '") + folder + "'.",
                                       folder,
                                       std::make_error_code(std::errc::permission_denied));
    }
}
