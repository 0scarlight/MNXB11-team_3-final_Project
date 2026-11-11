#pragma once

#include <filesystem>

void cleanSmhiDirectory(const std::filesystem::path& inputDirectory,
                        const std::filesystem::path& outputDirectory);
