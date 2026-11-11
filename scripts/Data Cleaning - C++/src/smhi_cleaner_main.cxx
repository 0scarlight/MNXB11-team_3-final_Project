#include "smhi_cleaner.h"

#include <exception>
#include <filesystem>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[])
{
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        std::cout << "Usage: ./smhi_cleaner [input_directory [output_directory]]\n"
                     "Defaults: datasets and results/data_cleaning\n";
        return 0;
    }
    if (argc > 3) {
        std::cerr << "Usage: ./smhi_cleaner [input_directory [output_directory]]\n";
        return 2;
    }

    try {
        cleanSmhiDirectory(argc > 1 ? std::filesystem::u8path(argv[1]) : "datasets",
                           argc > 2 ? std::filesystem::u8path(argv[2]) : "results/data_cleaning");
    } catch (const std::exception& error) {
        std::cerr << "SMHI cleaning failed: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
