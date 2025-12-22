#include <iostream>
#include "utils.hpp"
#include "tests.hpp"
#include "cli.hpp"
using namespace std;

void handleFlags(int &argc, char** &argv, bool &verbose) {
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
            // Remove the flag from argv
            for (int j = i; j < argc - 1; ++j) {
                argv[j] = argv[j + 1];
            }
            --argc;
            --i; // Adjust index after removal
        }
        else if (arg[0] == '-') {
            cout << "ignoring unknown flag: " << arg << endl;
        }
    }
}

int main(int argc, char** argv) {
    bool verbose = false;

    handleFlags(argc, argv, verbose);

    if (argc <= 1) {
        help();
        return 0;
    }

    const char* const command = argv[1];
    const string cmd_str(command);

    const char* const parameter = (argc > 2) ? argv[2] : "";

    if (cmd_str == "conjugatefactors") {
        const unsigned long word_length = strlen(parameter);
        if (word_length == 0) {
            cout << "Please provide a non-empty word." << endl;
            return 1;
        }
        PrintAllFactors(parameter, word_length, verbose);
    }
    else if (cmd_str == "showtrees") {
        showTrees(parameter, verbose);
    }
    else if (cmd_str == "test") {
        testAll();
    }
    else if (cmd_str == "help") {
        help();
    }
    else if (cmd_str == "suffixarray") {
        suffixArray(parameter, verbose);
    }
    else {
        cout << "Unknown command: " << cmd_str << endl;
        help();
        return 1;
    }
    return 0;
}