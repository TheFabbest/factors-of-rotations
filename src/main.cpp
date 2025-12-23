#include <iostream>
#include "utils.hpp"
#include "tests.hpp"
#include "cli.hpp"
#include "CLI/CLI11.hpp"
using namespace std;

struct Config {
    bool verbose = false;
    bool input_is_file = false;
    std::string input;
    std::string command;
    size_t bytes_per_char = 1;
    size_t max_chars = 0; // 0 = unlimited
};

int main(int argc, char** argv) {
    CLI::App app{"String algorithm tool"};
    Config config;
    
    // Global flags
    app.add_flag("-v,--verbose", config.verbose, "Verbose output");
    app.fallthrough();

    // Subcommands
    auto* conjugate = app.add_subcommand("conjugatefactors", "Find conjugate factors");
    conjugate->add_option("word", config.input, "Input word or filename")->required();
    
    auto* trees = app.add_subcommand("showtrees", "Show trees");
    trees->add_option("input", config.input, "Input string");
    
    auto* suffix = app.add_subcommand("suffixarray", "Build suffix array");
    suffix->add_option("input", config.input, "Input string");
    suffix->add_flag("--input-file", config.input_is_file, "Treat input as filename");
    suffix->add_option("--bytes-per-char", config.bytes_per_char, "Bytes per character");
    suffix->add_option("--max-chars", config.max_chars, "Max characters to read");
    
    auto* test = app.add_subcommand("test", "Run tests");
    
    CLI11_PARSE(app, argc, argv);
    
    const char* const word = config.input.c_str();
    const unsigned long length = config.input.length();

    // Execute based on subcommand
    if (*conjugate) {
        PrintAllFactors(word, config.verbose);
    }
    else if (*trees) {
        showTrees(word, config.verbose);
    }
    else if (*test) {
        testAll();
    }
    else if (*suffix) {
        if (config.input_is_file) {
            unsigned long size;
            unsigned long* file_content = readFile(config.input, config.bytes_per_char, config.max_chars, size);
            if (file_content) {
                if (config.bytes_per_char == sizeof(char)) {
                    char* char_content = reinterpret_cast<char*>(file_content);
                    suffixArray(char_content, size, config.verbose);
                    delete[] char_content;
                } else {
                    suffixArray(file_content, size, config.verbose);
                }
                delete[] file_content;
            }
        } else {
            suffixArray(word, length, config.verbose);
        }
    }
    else if (app.get_subcommands().size() > 0 && *(app.get_subcommands()[0]) ) {
        testAll();
    }
    return 0;
}