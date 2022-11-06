//
// Created by JinHai on 2022/9/7.
//

#include "compilation_config.h"
#include "linenoise.h"
#include "console.h"

#include <iostream>

int main(int argc, char** argv) {
    infinity::Console console;
    console.Init();

    std::cout << "Startup Infinity database console, version: "
              << VERSION_MAJOR << "."
              << VERSION_MINOR << "."
              << VERSION_PATCH << " built on "
              << BUILD_TIME << " from branch: "
              << GIT_BRANCH_NAME << std::endl;

    std::string history_file("command_history.log");

    // Use multiple line mode
    linenoiseSetMultiLine(1);

    // Store the command history into the file
    linenoiseHistoryLoad(history_file.c_str());

    char *line = nullptr;
    while((line = linenoise("infinity> ")) != nullptr) {
        try {
            auto result = console.HandleCommand(line);

            // Add command into history
            linenoiseHistoryAdd(result.c_str());

            // Flush the persist the history file.
            linenoiseHistorySave(history_file.c_str());
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
        free(line);
    }
}