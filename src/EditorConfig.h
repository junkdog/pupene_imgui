#pragma once

#include <string>

struct EditorConfig {
    struct Filter {
        std::string pattern;
        bool show_parents;
        bool request_focus;
    };

    std::string title;
    Filter filter = {};
};

